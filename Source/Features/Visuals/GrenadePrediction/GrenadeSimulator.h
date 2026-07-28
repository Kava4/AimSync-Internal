#pragma once

#include <algorithm>
#include <CS2/Classes/Vector.h>
#include <GameClient/EngineTrace/EngineTrace.h>
#include <Utils/Math.h>
#include <Utils/Optional.h>
#include <Features/Visuals/GrenadePrediction/GrenadeKind.h>
#include <Features/Visuals/GrenadePrediction/Trajectory.h>
#include <Features/Visuals/GrenadePrediction/GrenadePredictionParams.h>

struct StepResult {
    bool traceSucceeded = true;
    bool impactDetonate = false;
    bool hit = false;
    cs2::Vector hitPos{};
    int contactsCount{};
    cs2::Vector contacts[grenade_prediction_params::kMaxCollisionPasses + 1]{};
};

template <typename HookContext>
struct GrenadeSimulatorTestAccess;

template <typename HookContext>
class GrenadeSimulator {
public:
    explicit GrenadeSimulator(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    // NOLINTNEXTLINE(readability-function-cognitive-complexity)
    void simulate(Trajectory& traj, cs2::Vector start, cs2::Vector velocity, cs2::GrenadeKind kind, void* skipEntity, float frictionOverride = 0.0f) noexcept
    {
        using namespace grenade_prediction_params;

        traj.pointsCount = 0;
        traj.bouncesCount = 0;
        traj.endPos = start;
        traj.valid = false;
        traj.validLanding = true;

        cs2::Vector pos = start;
        cs2::Vector vel = velocity;
        int bounceCount = 0;
        int tickTimer = 0;
        bool landedOnSurface = false;

        for (int tick = 0; tick < kMaxTicks; ++tick) {
            if (tickTimer == 0) {
                if (traj.pointsCount < 500)
                    traj.points[traj.pointsCount++] = pos;
            }

            cs2::Vector prevPos = pos;
            StepResult result = step(pos, vel, kind, skipEntity, frictionOverride);

            if (!result.traceSucceeded) {
                traj.pointsCount = 0;
                traj.bouncesCount = 0;
                traj.endPos = start;
                traj.validLanding = true;
                return;
            }

            for (int contact = 0; contact < result.contactsCount; ++contact) {
                ++bounceCount;
                if (traj.bouncesCount < 20)
                    traj.bounces[traj.bouncesCount++] = result.contacts[contact];
            }
            if (result.impactDetonate)
                landedOnSurface = true;

            bool displacementStopped = false;
            if (kind == cs2::GrenadeKind::SmokeGrenade || kind == cs2::GrenadeKind::Decoy) {
                cs2::Vector disp = pos - prevPos;
                displacementStopped = disp.squareLength() < kStopDisplacementSq;
            }

            if (result.impactDetonate || shouldDetonate(kind, tick) || bounceCount > kMaxBounces || displacementStopped) {
                traj.endPos = pos;
                traj.valid = true;

                if (kind == cs2::GrenadeKind::Molotov || kind == cs2::GrenadeKind::Incendiary)
                    traj.validLanding = landedOnSurface || result.impactDetonate;

                break;
            }

            if (result.hit || ++tickTimer >= kTicksPerPoint) {
                tickTimer = 0;
            }
        }

        if (traj.valid && traj.pointsCount > 0 && traj.points[traj.pointsCount - 1].squareDistTo(traj.endPos) > 1.0f) {
            if (traj.pointsCount < 500)
                traj.points[traj.pointsCount++] = traj.endPos;
        }
    }

    static cs2::Vector computeInitialVelocity(cs2::Vector viewAngles, float baseVelocity, float throwStrength) noexcept
    {
        float strength = normalizeThrowStrength(throwStrength);

        float pitch = viewAngles.x;
        float yaw = viewAngles.y;

        float correctedPitch = pitch - (90.0f - Math::abs(pitch)) * 10.0f / 90.0f;

        float throwVelocity = std::clamp(baseVelocity * 0.9f, 15.0f, 750.0f);
        float throwSpeed = (strength * 0.7f + 0.3f) * throwVelocity;

        return forwardFromAngles(correctedPitch, yaw) * throwSpeed;
    }

    [[nodiscard]] Optional<cs2::Vector> computeSpawnPosition(cs2::Vector eyePos, cs2::Vector viewAngles, float throwStrength, void* skipEntity) noexcept
    {
        using namespace grenade_prediction_params;

        float strength = normalizeThrowStrength(throwStrength);

        float pitch = viewAngles.x;
        float yaw = viewAngles.y;
        float correctedPitch = pitch - (90.0f - Math::abs(pitch)) * 10.0f / 90.0f;

        cs2::Vector forward = forwardFromAngles(correctedPitch, yaw);

        eyePos.z += (strength * kThrowZOffsetScale - kThrowZOffsetScale);

        cs2::Vector traceEnd = eyePos + forward * kSpawnTraceForward;
        auto traceResult = traceGrenadeHull(eyePos, traceEnd, skipEntity);

        if (!traceResult.hasValue())
            return {};

        cs2::Vector hitPos;
        if (traceResult.value().fraction < 1.0f) {
            hitPos = traceResult.value().endPos;
        } else {
            hitPos = traceEnd;
        }

        cs2::Vector spawnPos = hitPos - forward * kSpawnPullBack;
        cs2::Vector eyeToSpawn = spawnPos - eyePos;
        if (eyeToSpawn.dot(forward) < 0.0f)
            spawnPos = eyePos;
        return Optional<cs2::Vector>{spawnPos};
    }

    static cs2::Vector clipVelocity(cs2::Vector velocity, cs2::Vector normal, float overbounce) noexcept
    {
        float dot = velocity.dot(normal);
        if (dot >= 0.0f)
            return velocity;

        float backoff = -dot * overbounce + grenade_prediction_params::kClipPushOff;
        return cs2::Vector{
            velocity.x + normal.x * backoff,
            velocity.y + normal.y * backoff,
            velocity.z + normal.z * backoff
        };
    }

    static cs2::Vector forwardFromAngles(float pitch, float yaw) noexcept
    {
        float p = pitch * 3.14159265f / 180.0f;
        float y = yaw * 3.14159265f / 180.0f;
        float sp, cp, sy, cy;
        Math::sincos(p, sp, cp);
        Math::sincos(y, sy, cy);
        return cs2::Vector{cp * cy, cp * sy, -sp};
    }

    static float normalizeThrowStrength(float strength) noexcept
    {
        return strength > 0.4f && strength < 0.6f ? 0.5f : strength;
    }

private:
    struct CollisionResult {
        bool traceSucceeded = true;
        bool impactDetonate = false;
        bool stopped = false;
    };

    [[nodiscard]] Optional<TraceResult> traceGrenadeHull(cs2::Vector start, cs2::Vector end, void* skipEntity) noexcept
    {
        return hookContext.template make<EngineTrace<HookContext>>().traceGrenadeHull(start, end, skipEntity);
    }

    StepResult step(cs2::Vector& pos, cs2::Vector& vel, cs2::GrenadeKind kind, void* skipEntity, float frictionOverride) noexcept
    {
        using namespace grenade_prediction_params;

        auto physics = getGrenadePhysics(kind);
        float gravity = kSvGravity * physics.gravityScale;
        float oldVelZ = vel.z;
        vel.z -= gravity * kSimDt;
        cs2::Vector moveVec{vel.x * kSimDt, vel.y * kSimDt, (oldVelZ + vel.z) * 0.5f * kSimDt};

        cs2::Vector endPos = pos + moveVec;

        auto traceResult = traceGrenadeHull(pos, endPos, skipEntity);

        if (!traceResult.hasValue())
            return {.traceSucceeded = false};

        if (traceResult.value().fraction >= 1.0f) {
            pos = endPos;
            return {};
        }

        cs2::Vector exactHitPos = traceResult.value().endPos;
        pos = exactHitPos;
        StepResult result;
        result.hit = true;
        result.hitPos = exactHitPos;
        result.contacts[result.contactsCount++] = exactHitPos;
        const auto collisionResult = resolveCollision(traceResult.value(), pos, vel, kind, skipEntity, frictionOverride, result);
        result.traceSucceeded = collisionResult.traceSucceeded;
        result.impactDetonate = collisionResult.impactDetonate;
        return result;
    }

    CollisionResult applyContactResponse(const TraceResult& trace, cs2::Vector& vel, cs2::GrenadeKind kind, float frictionOverride) noexcept
    {
        using namespace grenade_prediction_params;

        if ((kind == cs2::GrenadeKind::Molotov || kind == cs2::GrenadeKind::Incendiary) &&
            (trace.normal.z >= kMolotovSlope || vel.squareLength() < kStopSpeedSq)) {
            vel = cs2::Vector{0.0f, 0.0f, 0.0f};
            return {.impactDetonate = true, .stopped = true};
        }

        auto physics = getGrenadePhysics(kind);
        cs2::Vector newVel = clipVelocity(vel, trace.normal, 2.0f);
        newVel = newVel * physics.elasticity;

        if (frictionOverride > 0.0f) {
            float normalSpeed = newVel.dot(trace.normal);
            cs2::Vector normalComp{trace.normal.x * normalSpeed, trace.normal.y * normalSpeed, trace.normal.z * normalSpeed};
            cs2::Vector tangentComp{newVel.x - normalComp.x, newVel.y - normalComp.y, newVel.z - normalComp.z};

            float frictionRetain = 1.0f - frictionOverride;
            tangentComp.x *= frictionRetain;
            tangentComp.y *= frictionRetain;
            tangentComp.z *= frictionRetain;

            newVel = cs2::Vector{normalComp.x + tangentComp.x, normalComp.y + tangentComp.y, normalComp.z + tangentComp.z};
        }

        if (newVel.squareLength() < kStopSpeedSq) {
            vel = cs2::Vector{0.0f, 0.0f, 0.0f};
            return {.stopped = true};
        }

        vel = newVel;
        return {};
    }

    CollisionResult resolveCollision(const TraceResult& trace, cs2::Vector& pos, cs2::Vector& vel, cs2::GrenadeKind kind, void* skipEntity, float frictionOverride, StepResult& result) noexcept
    {
        using namespace grenade_prediction_params;

        auto collisionResult = applyContactResponse(trace, vel, kind, frictionOverride);
        if (collisionResult.impactDetonate || collisionResult.stopped)
            return collisionResult;

        float remaining = 1.0f - trace.fraction;
        for (int pass = 0; pass < kMaxCollisionPasses && remaining > kRemainingFractionEpsilon; ++pass) {
            cs2::Vector postEnd = pos + (vel * (remaining * kSimDt));
            auto post = traceGrenadeHull(pos, postEnd, skipEntity);

            if (!post.hasValue())
                return {.traceSucceeded = false};

            if (post.value().fraction >= 1.0f) {
                pos = postEnd;
                break;
            }

            pos = post.value().endPos;
            remaining *= (1.0f - post.value().fraction);
            result.contacts[result.contactsCount++] = pos;
            collisionResult = applyContactResponse(post.value(), vel, kind, frictionOverride);
            if (collisionResult.impactDetonate || collisionResult.stopped)
                return collisionResult;
        }

        return {};
    }

    bool shouldDetonate(cs2::GrenadeKind kind, int tick) const noexcept
    {
        using namespace grenade_prediction_params;

        float elapsed = static_cast<float>(tick) * kSimDt;
        switch (kind) {
            case cs2::GrenadeKind::SmokeGrenade:
                return elapsed > kDetonateTimeSmokeCap;
            case cs2::GrenadeKind::Decoy:
                return elapsed > kDetonateTimeDecoy;
            case cs2::GrenadeKind::Molotov:
            case cs2::GrenadeKind::Incendiary:
                return elapsed > kDetonateTimeMolotov;
            case cs2::GrenadeKind::Flashbang:
            case cs2::GrenadeKind::HEGrenade:
                return elapsed > kDetonateTimeHeFlash;
            default:
                return false;
        }
    }

    HookContext& hookContext;

    friend struct GrenadeSimulatorTestAccess<HookContext>;
};
