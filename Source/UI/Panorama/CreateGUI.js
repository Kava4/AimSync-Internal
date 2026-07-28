u8R"(
$.AimSync = (function () {
  var activeTab;
  var activeSubTab = {};

  return {
    rootPanel: (function () {
      const rootPanel = $.CreatePanel('Panel', $.GetContextPanel(), 'AimSyncMenuTab', {
        class: "mainmenu-content__container",
        useglobalcontext: "true"
      });

      rootPanel.visible = false;
      rootPanel.SetReadyForDisplay(false);
      rootPanel.RegisterForReadyEvents(true);
      $.RegisterEventHandler('PropertyTransitionEnd', rootPanel, function (panelName, propertyName) {
        if (rootPanel.id === panelName && propertyName === 'opacity') {
          if (rootPanel.visible === true && rootPanel.BIsTransparent()) {
            rootPanel.visible = false;
            rootPanel.SetReadyForDisplay(false);
            return true;
          } else if (rootPanel.visible === true) {
            $.DispatchEvent('MainMenuTabShown', 'AimSyncMenuTab');
          }
        }
        return false;
      });

      return rootPanel;
    })(),
    goHome: function () {
      $.DispatchEvent('Activated', this.rootPanel.GetParent().GetParent().GetParent().FindChildInLayoutFile("MainMenuNavBarHome"), 'mouse');
    },
    addCommand: function (command, value = '') {
      var existingCommands = this.rootPanel.GetAttributeString('cmd', '');
      this.rootPanel.SetAttributeString('cmd', existingCommands + command + ' ' + value);
    },
    navigateToTab: function (tabID) {
      if (activeTab === tabID)
        return;

      if (activeTab) {
        var panelToHide = this.rootPanel.FindChildInLayoutFile(activeTab);
        panelToHide.RemoveClass('Active');
      }

      this.rootPanel.FindChildInLayoutFile(tabID + '_button').checked = true;

      activeTab = tabID;
      var activePanel = this.rootPanel.FindChildInLayoutFile(tabID);
      activePanel.AddClass('Active');
      activePanel.visible = true;
      activePanel.SetReadyForDisplay(true);
    },
    navigateToSubTab: function (tabID, subTabID) {
      if (activeSubTab[tabID] === subTabID)
        return;

      if (activeSubTab[tabID]) {
        var panelToHide = this.rootPanel.FindChildInLayoutFile(activeSubTab[tabID]);
        panelToHide.RemoveClass('Active');
      }

      this.rootPanel.FindChildInLayoutFile(subTabID + '_button').checked = true;

      activeSubTab[tabID] = subTabID;
      var activePanel = this.rootPanel.FindChildInLayoutFile(subTabID);
      activePanel.AddClass('Active');
      activePanel.visible = true;
      activePanel.SetReadyForDisplay(true);
    },
    sliderUpdated: function (tabID, sliderID, slider) {
      this.addCommand('set', tabID + '/' + sliderID + '/' + Math.floor(slider.value));
    },
    sliderTextEntryUpdated: function (tabID, sliderID, panel) {
      this.addCommand('set', tabID + '/' + sliderID + '/' + panel.text);
    }
  };
})();

(function () {
  var accentColor = "#5eb3f6ff";
  var accentColorDim = "#5eb3f650";
  var accentColorBorder = "#5eb3f630";
  var brandFont = "Stratum2, notosans, 'Arial Unicode MS'";
  var brandIcon = "s2r://panorama/images/icons/ui/chatwheel_sniperspotted.svg";
  var tabLabelStyle = "color: " + accentColor + "; font-weight: bold;";

  var createNavbar = function () {
    var navbar = $.CreatePanel('Panel', $.AimSync.rootPanel, '', {
      class: "content-navbar__tabs content-navbar__tabs--noflow"
    });

    var leftContainer = $.CreatePanel('Panel', navbar, '', {
      style: "horizontal-align: left; flow-children: right; height: 100%; margin-left: 15px;"
    });

    var brandContainer = $.CreatePanel('Panel', leftContainer, '', {
      style: "flow-children: right; vertical-align: center; margin-right: 20px;"
    });

    $.CreatePanel('Image', brandContainer, '', {
      src: brandIcon,
      texturewidth: "28",
      style: "margin-right: 8px; vertical-align: center;"
    });

    var brandTextContainer = $.CreatePanel('Panel', brandContainer, '', {
      style: "flow-children: down; vertical-align: center;"
    });

    $.CreatePanel('Label', brandTextContainer, 'AimSyncBrandLabel', {
      text: "AimSync",
      style: "font-size: 24px; font-weight: bold; color: " + accentColor + "; font-family: " + brandFont + ";"
    });

    $.CreatePanel('Label', brandTextContainer, 'AimSyncBrandSubtitle', {
      text: "Internal · CS2",
      style: "font-size: 13px; color: #999999ff; font-family: " + brandFont + "; margin-top: -2px;"
    });

    var activeCfgNameLabel = $.CreatePanel('Label', leftContainer, 'ActiveConfigName', {
      text: "default.cfg",
      style: "color: #ccccccff; font-size: 18px; font-family: " + brandFont + ";"
    });

    activeCfgNameLabel.SetPanelEvent('onmouseover', function () { UiToolkitAPI.ShowTextTooltip('ActiveConfigName', 'AimSync Internal CS2 active config. Changes are saved automatically.'); });
    activeCfgNameLabel.SetPanelEvent('onmouseout', function () { UiToolkitAPI.HideTextTooltip(); });

    var restoreDefaultsButton = $.CreatePanel('Button', leftContainer, 'RestoreDefaultsButton', {
      class: "content-navbar__tabs__btn",
      style: "margin-left: 5px;",
      onactivate: "UiToolkitAPI.ShowGenericPopupOneOptionCustomCancelBgStyle('AimSync Internal CS2 — Restore Defaults', 'Reset all AimSync Internal CS2 settings in the active config (default.cfg) to factory defaults?', '', 'RESTORE', function() { $.AimSync.addCommand('restore_defaults'); }, 'CANCEL', function() {}, 'dim');"
    });

    restoreDefaultsButton.SetPanelEvent('onmouseover', function () { UiToolkitAPI.ShowTextTooltip('RestoreDefaultsButton', 'Restore AimSync Internal CS2 defaults'); });
    restoreDefaultsButton.SetPanelEvent('onmouseout', function () { UiToolkitAPI.HideTextTooltip(); });

    $.CreatePanel('Image', restoreDefaultsButton, '', {
      src: "s2r://panorama/images/icons/ui/recent.vsvg",
      texturewidth: "24"
    });
  
    var centerContainer = $.CreatePanel('Panel', navbar, '', {
      class: "content-navbar__tabs__center-container",
    });

    var combatTabButton = $.CreatePanel('RadioButton', centerContainer, 'combat_button', {
      group: "SettingsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.AimSync.navigateToTab('combat');"
    });

    $.CreatePanel('Label', combatTabButton, '', { text: "Aim", style: tabLabelStyle });

    var hudTabButton = $.CreatePanel('RadioButton', centerContainer, 'hud_button', {
      group: "SettingsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.AimSync.navigateToTab('hud');"
    });

    $.CreatePanel('Label', hudTabButton, '', { text: "Overlay", style: tabLabelStyle });

    var visualsTabButton = $.CreatePanel('RadioButton', centerContainer, 'visuals_button', {
      group: "SettingsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.AimSync.navigateToTab('visuals');"
    });

    $.CreatePanel('Label', visualsTabButton, '', { text: "ESP", style: tabLabelStyle });
    
    var soundTabButton = $.CreatePanel('RadioButton', centerContainer, 'sound_button', {
      group: "SettingsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.AimSync.navigateToTab('sound');"
    });

    $.CreatePanel('Label', soundTabButton, '', { text: "Audio", style: tabLabelStyle });

    var rightContainer = $.CreatePanel('Panel', navbar, '', {
        style: "horizontal-align: right; flow-children: right; height: 100%; margin-right: 70px;"
    });

    var unloadButton = $.CreatePanel('Button', rightContainer, 'UnloadButton', {
        class: "content-navbar__tabs__btn",
        onactivate: "UiToolkitAPI.ShowGenericPopupOneOptionCustomCancelBgStyle('Unload AimSync Internal CS2', 'AimSync Internal CS2 will be removed from memory. Continue?', '', 'UNLOAD', function() { $.AimSync.goHome(); $.AimSync.addCommand('unload'); }, 'CANCEL', function() {}, 'dim');"
    });

    unloadButton.SetPanelEvent('onmouseover', function () { UiToolkitAPI.ShowTextTooltip('UnloadButton', 'Unload AimSync Internal CS2'); });
    unloadButton.SetPanelEvent('onmouseout', function () { UiToolkitAPI.HideTextTooltip(); });

    $.CreatePanel('Image', unloadButton, '', {
        src: "s2r://panorama/images/icons/ui/cancel.vsvg",
        texturewidth: "24",
        class: "negativeColor"
    });
  };

  var createVisualsNavbar = function () {
    var navbar = $.CreatePanel('Panel', $.AimSync.rootPanel.FindChildInLayoutFile('visuals'), '', {
      class: "content-navbar__tabs content-navbar__tabs--dark content-navbar__tabs--noflow"
    });

    var centerContainer = $.CreatePanel('Panel', navbar, '', {
      class: "content-navbar__tabs__center-container",
    });

    var playerInfoTabButton = $.CreatePanel('RadioButton', centerContainer, 'player_info_button', {
      group: "VisualsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.AimSync.navigateToSubTab('visuals', 'player_info');"
    });

    $.CreatePanel('Label', playerInfoTabButton, '', { text: "Player ESP", style: tabLabelStyle });

    var outlineGlowTabButton = $.CreatePanel('RadioButton', centerContainer, 'outline_glow_button', {
      group: "VisualsNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.AimSync.navigateToSubTab('visuals', 'outline_glow');"
    });

    $.CreatePanel('Label', outlineGlowTabButton, '', { text: "Outlines", style: tabLabelStyle });

    var modelGlowTabButton = $.CreatePanel('RadioButton', centerContainer, 'model_glow_button', {
        group: "VisualsNavBar",
        class: "content-navbar__tabs__btn",
        onactivate: "$.AimSync.navigateToSubTab('visuals', 'model_glow');"
    });

    $.CreatePanel('Label', modelGlowTabButton, '', { text: "Models", style: tabLabelStyle });
  
    var viewmodelTabButton = $.CreatePanel('RadioButton', centerContainer, 'viewmodel_button', {
        group: "VisualsNavBar",
        class: "content-navbar__tabs__btn",
        onactivate: "$.AimSync.navigateToSubTab('visuals', 'viewmodel');"
    });

    $.CreatePanel('Label', viewmodelTabButton, '', { text: "Viewmodel", style: tabLabelStyle });

    var grenadePredictionTabButton = $.CreatePanel('RadioButton', centerContainer, 'grenade_prediction_button', {
        group: "VisualsNavBar",
        class: "content-navbar__tabs__btn",
        onactivate: "$.AimSync.navigateToSubTab('visuals', 'grenade_prediction');"
    });

    $.CreatePanel('Label', grenadePredictionTabButton, '', { text: "Grenade Pred.", style: tabLabelStyle });
  };

  var createCombatNavbar = function () {
    var navbar = $.CreatePanel('Panel', $.AimSync.rootPanel.FindChildInLayoutFile('combat'), '', {
      class: "content-navbar__tabs content-navbar__tabs--dark content-navbar__tabs--noflow"
    });

    var centerContainer = $.CreatePanel('Panel', navbar, '', {
      class: "content-navbar__tabs__center-container",
    });

    var sniperRiflesTabButton = $.CreatePanel('RadioButton', centerContainer, 'sniper_rifles_button', {
      group: "CombatNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.AimSync.navigateToSubTab('combat', 'sniper_rifles');"
    });

    $.CreatePanel('Label', sniperRiflesTabButton, '', { text: "Snipers", style: tabLabelStyle });

    var aimbotTabButton = $.CreatePanel('RadioButton', centerContainer, 'aimbot_button', {
      group: "CombatNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.AimSync.navigateToSubTab('combat', 'aimbot');"
    });

    $.CreatePanel('Label', aimbotTabButton, '', { text: "Aimbot", style: tabLabelStyle });

    var noRecoilTabButton = $.CreatePanel('RadioButton', centerContainer, 'no_recoil_button', {
      group: "CombatNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.AimSync.navigateToSubTab('combat', 'no_recoil');"
    });

    $.CreatePanel('Label', noRecoilTabButton, '', { text: "No Recoil", style: tabLabelStyle });

    var triggerbotTabButton = $.CreatePanel('RadioButton', centerContainer, 'triggerbot_button', {
      group: "CombatNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.AimSync.navigateToSubTab('combat', 'triggerbot');"
    });

    $.CreatePanel('Label', triggerbotTabButton, '', { text: "Trigger", style: tabLabelStyle });

    var skinsTabButton = $.CreatePanel('RadioButton', centerContainer, 'skins_button', {
      group: "CombatNavBar",
      class: "content-navbar__tabs__btn",
      onactivate: "$.AimSync.navigateToSubTab('combat', 'skins');"
    });

    $.CreatePanel('Label', skinsTabButton, '', { text: "Skins", style: tabLabelStyle });
  };

  createNavbar();

  var settingContent = $.CreatePanel('Panel', $.AimSync.rootPanel, 'SettingsMenuContent', {
    class: "full-width full-height"
  });

  var createTab = function(tabName) {
    var tab = $.CreatePanel('Panel', settingContent, tabName, {
      useglobalcontext: "true",
      class: "SettingsMenuTab"
    });

    var content = $.CreatePanel('Panel', tab, '', {
      class: "SettingsMenuTabContent vscroll"
    });
  
    return content;
  };

  var createVisualsTab = function() {
    var tab = $.CreatePanel('Panel', settingContent, 'visuals', {
      useglobalcontext: "true",
      class: "SettingsMenuTab"
    });

    createVisualsNavbar();

    var content = $.CreatePanel('Panel', tab, '', {
      class: "full-width full-height"
    });
  
    return content;
  };

  var createCombatTab = function() {
    var tab = $.CreatePanel('Panel', settingContent, 'combat', {
      useglobalcontext: "true",
      class: "SettingsMenuTab"
    });

    createCombatNavbar();

    var content = $.CreatePanel('Panel', tab, '', {
      class: "full-width full-height"
    });
  
    return content;
  };

  var createSubTab = function(tab, subTabName) {
    var subTab = $.CreatePanel('Panel', tab, subTabName, {
      useglobalcontext: "true",
      class: "SettingsMenuTab"
    });

    var content = $.CreatePanel('Panel', subTab, '', {
      class: "SettingsMenuTabContent vscroll"
    });
    return content;
  };

  var createSection = function(tab, sectionName) {
    var background = $.CreatePanel('Panel', tab, '', {
      class: "SettingsBackground"
    });

    var titleContainer = $.CreatePanel('Panel', background, '', {
      class: "SettingsSectionTitleContianer"
    });

    $.CreatePanel('Label', titleContainer, '', {
      class: "SettingsSectionTitleLabel",
      text: sectionName,
      style: "color: " + accentColor + ";"
    });

    var content = $.CreatePanel('Panel', background, '', {
      class: "top-bottom-flow full-width"
    });

    return content;
  };

  var createWipSection = function(tab, featureName) {
    var section = createSection(tab, featureName);
    $.CreatePanel('Label', section, '', {
      text: "Work in progress — coming soon.",
      style: "color: #ccccccff; font-size: 16px; padding: 12px 16px;"
    });
    return section;
  };
)"
// split the string literal because MSVC does not support string literals longer than 16k chars - error C2026
u8R"(
  var createDropDown = function (parent, labelText, section, feature, options) {
    var container = $.CreatePanel('Panel', parent, '', {
      class: "SettingsMenuDropdownContainer"
    });

    $.CreatePanel('Label', container, '', {
      class: "half-width",
      text: labelText
    });

    var dropdown = $.CreatePanel('CSGOSettingsEnumDropDown', container, feature, { class: "PopupButton White" });

    for (let i = 0; i < options.length; ++i) {
      dropdown.AddOption($.CreatePanel('Label', dropdown, i, {
      value: i,
      text: options[i]
      }));
    }
  };

  var createOnOffDropDown = function (parent, labelText, section, feature) {
    createDropDown(parent, labelText, section, feature, ["On", "Off"]);
  };

  var createYesNoDropDown = function (parent, labelText, section, feature) {
    createDropDown(parent, labelText, section, feature, ["Yes", "No"]);
  };

  var separator = function (parent) {
    $.CreatePanel('Panel', parent, '', { class: "horizontal-separator" });
  };

  var createPaintKitTextEntry = function(parent, labelText, id) {
    var container = $.CreatePanel('Panel', parent, '', {
      class: "SettingsMenuDropdownContainer"
    });

    $.CreatePanel('Label', container, '', {
      class: "half-width",
      text: labelText
    });

    var textEntry = $.CreatePanel('TextEntry', container, id, {
      maxchars: "5",
      textmode: "numeric",
      style: "width: 120px; horizontal-align: right; margin-right: 8px; padding-left: 10px; text-align: center; font-size: 18px; color: #ccccccff; font-weight: bold; font-family: " + brandFont + "; border: 2px solid " + accentColorBorder + ";"
    });

    textEntry.SetPanelEvent('ontextentrysubmit', function () {
      $.AimSync.addCommand('set', 'combat/' + id + '_text/' + textEntry.text);
    });
  };

  var makeFauxItemId = function (defIndex, paintKitId) {
    return (BigInt(0xF000000000000000) | BigInt(paintKitId << 16) | BigInt(defIndex))
  };

  var createPlayerModelGlowPreview = function (parent, id, labelId, playerModel, itemId) {
    var container = $.CreatePanel('Panel', parent, '', { style: 'flow-children: none;' });
    var previewPanel = $.CreatePanel('MapPlayerPreviewPanel', container, id, {
      map: "ui/buy_menu",
      camera: "cam_loadoutmenu_ct",
      "require-composition-layer": true,
      playermodel: playerModel,
      playername: "vanity_character",
      animgraphcharactermode: "buy-menu",
      player: true,
      mouse_rotate: false,
      sync_spawn_addons: true,
      "transparent-background": true,
      "pin-fov": "vertical",
      csm_split_plane0_distance_override: "250.0",
      style: "y: 5px; vertical-align: top; width: 300px; height: 300px; horizontal-align: center;"
    });
    previewPanel.EquipPlayerWithItem(itemId);
    $.CreatePanel('Label', container, labelId, { style: 'vertical-align: top; horizontal-align: center;' });
  };

  var createGrenadeModelGlowPreview = function (parent, id, defIndex) {
    var container = $.CreatePanel('Panel', parent, '', { style: 'width: 80px; overflow: clip;' });
    var panel = $.CreatePanel('MapItemPreviewPanel', container, id, {
      map: "ui/xpshop_item",
      camera: "camera_weapon_4",
      "require-composition-layer": true,
      player: false,
      initial_entity: "item",
      mouse_rotate: false,
      sync_spawn_addons: true,
      "transparent-background": true,
      "pin-fov": "vertical",
      style: "x: -10px; horizontal-align: center; width: 200px; height: 80px;"
    });
    panel.SetItemItemId(makeFauxItemId(defIndex, 0), {});
  };
)"
// split the string literal because MSVC does not support string literals longer than 16k chars - error C2026
u8R"(
  var createSlider = function (parent, name, id, min, max, section) {
    section = section || 'visuals';
    var container = $.CreatePanel('Panel', parent, '', {
      class: "SettingsMenuDropdownContainer"
    });

    $.CreatePanel('Label', container, '', {
      class: "half-width",
      text: name
    });

    var sliderContainer = $.CreatePanel('Panel', container, id, {
      style: "vertical-align: center; horizontal-align: right; flow-children: right; margin-right: 8px;"
    });

    var slider = $.CreatePanel('Slider', sliderContainer, '', {
      class: "HorizontalSlider",
      style: "width: 200px; vertical-align: center;",
      direction: "horizontal"
    });

    slider.SetPanelEvent('onvaluechanged', function () { $.AimSync.sliderUpdated(section, id, slider); });
    slider.min = min;
    slider.max = max;
    slider.increment = 1.0;

    var textEntry = $.CreatePanel('TextEntry', sliderContainer, id + '_text', {
      maxchars: "3",
      textmode: "numeric",
      style: "width: 75px; margin-left: 10px; padding-left: 10px; text-align: center; font-size: 20px; color: #ccccccff; font-weight: bold; font-family: " + brandFont + "; border: 2px solid " + accentColorBorder + ";"
    });

    textEntry.SetPanelEvent('ontextentrysubmit', function () { $.AimSync.sliderTextEntryUpdated(section, `${id}_text`, textEntry); });
    textEntry.SetPanelEvent('onfocus', function () { textEntry.style.backgroundColor = 'gradient(linear, 100% 0%, 0% 0%, from(' + accentColorDim + '), color-stop(0, #00000060), to(#00000080))'; });
    textEntry.SetPanelEvent('onblur', function () { textEntry.style.backgroundColor = 'none'; });
    textEntry.SetPanelEvent('onmouseover', function () { if (!textEntry.BHasKeyFocus()) textEntry.style.backgroundColor = 'gradient(linear, 100% 0%, 0% 0%, from(' + accentColorDim + '), color-stop(0, #00000000), to(#00000050));'; });
    textEntry.SetPanelEvent('onmouseout', function () { if (!textEntry.BHasKeyFocus()) textEntry.style.backgroundColor = 'none'; });
  }

  var createHueSlider = function (parent, name, id, min, max) {
    var container = $.CreatePanel('Panel', parent, '', {
      class: "SettingsMenuDropdownContainer"
    });

    $.CreatePanel('Label', container, '', {
      class: "half-width",
      text: name
    });

    var sliderContainer = $.CreatePanel('Panel', container, id, {
      style: "vertical-align: center; horizontal-align: right; flow-children: right; margin-right: 8px;"
    });

    var slider = $.CreatePanel('Slider', sliderContainer, '', {
      class: "HorizontalSlider",
      style: "width: 200px; vertical-align: center;",
      direction: "horizontal"
    });

    slider.min = min;
    slider.max = max;
    slider.increment = 1.0;

    var textEntry = $.CreatePanel('TextEntry', sliderContainer, id + '_text', {
      maxchars: "3",
      textmode: "numeric",
      style: "width: 75px; margin-left: 10px; padding-left: 10px; text-align: center; font-size: 20px; color: #ccccccff; font-weight: bold; font-family: " + brandFont + "; border: 2px solid " + accentColorBorder + ";"
    });

    textEntry.SetPanelEvent('onfocus', function () { textEntry.style.backgroundColor = 'gradient(linear, 100% 0%, 0% 0%, from(' + accentColorDim + '), color-stop(0, #00000060), to(#00000080))'; });
    textEntry.SetPanelEvent('onblur', function () { textEntry.style.backgroundColor = 'none'; });
    textEntry.SetPanelEvent('onmouseover', function () { if (!textEntry.BHasKeyFocus()) textEntry.style.backgroundColor = 'gradient(linear, 100% 0%, 0% 0%, from(' + accentColorDim + '), color-stop(0, #00000000), to(#00000050));'; });
    textEntry.SetPanelEvent('onmouseout', function () { if (!textEntry.BHasKeyFocus()) textEntry.style.backgroundColor = 'none'; });

    $.CreatePanel('Panel', sliderContainer, id + '_color', {
        style: "border: 2px solid " + accentColor + "; border-radius: 5px; margin-left: 10px; width: 25px; vertical-align: center; height: 25px;"
    });
  }
)"
// split the string literal because MSVC does not support string literals longer than 16k chars - error C2026
u8R"(
  var combat = createCombatTab();
  var sniperRiflesTab = createSubTab(combat, 'sniper_rifles');
  var noScope = createSection(sniperRiflesTab, 'Scope Accuracy');
  separator(noScope);
  createYesNoDropDown(noScope, "Visualize Inaccuracy When Not Using a Scope", 'combat', 'no_scope_inacc_vis');

  var aimbotTab = createSubTab(combat, 'aimbot');
  var aimbot = createSection(aimbotTab, 'Aimbot');
  createOnOffDropDown(aimbot, "Enable Aimbot", 'combat', 'aimbot_enable');
  separator(aimbot);
  createYesNoDropDown(aimbot, "Ignore Teammates", 'combat', 'aimbot_team_check');
  separator(aimbot);
  createYesNoDropDown(aimbot, "Visibility Check", 'combat', 'aimbot_vis_check');
  separator(aimbot);
  createYesNoDropDown(aimbot, "Recoil Control", 'combat', 'aimbot_rcs');
  separator(aimbot);
  createYesNoDropDown(aimbot, "Only When Scoped", 'combat', 'aimbot_scoped_only');
  separator(aimbot);
  createYesNoDropDown(aimbot, "Disable When Flashed", 'combat', 'aimbot_no_flash');
  separator(aimbot);
  createSlider(aimbot, "FOV", 'aimbot_fov', 1, 30, 'combat');
  separator(aimbot);
  createSlider(aimbot, "Smoothness", 'aimbot_smooth', 1, 50, 'combat');

  var noRecoilTab = createSubTab(combat, 'no_recoil');
  var noRecoil = createSection(noRecoilTab, 'No Recoil');
  createOnOffDropDown(noRecoil, "Enable No Recoil", 'combat', 'no_recoil_enable');
  separator(noRecoil);
  createSlider(noRecoil, "Strength [%]", 'no_recoil_strength', 1, 100, 'combat');
  separator(noRecoil);
  createOnOffDropDown(noRecoil, "No Spread", 'combat', 'no_spread_enable');

  var triggerbotTab = createSubTab(combat, 'triggerbot');
  var triggerbot = createSection(triggerbotTab, 'Triggerbot');
  createOnOffDropDown(triggerbot, "Enable Triggerbot", 'combat', 'triggerbot_enable');
  separator(triggerbot);
  createYesNoDropDown(triggerbot, "Ignore Teammates", 'combat', 'triggerbot_team_check');
  separator(triggerbot);
  createYesNoDropDown(triggerbot, "Only When Scoped", 'combat', 'triggerbot_scoped_only');
  separator(triggerbot);
  createYesNoDropDown(triggerbot, "Disable When Flashed", 'combat', 'triggerbot_no_flash');
  separator(triggerbot);
  createSlider(triggerbot, "Delay [ms]", 'triggerbot_delay', 0, 250, 'combat');

  var skinsTab = createSubTab(combat, 'skins');
  var inventorySection = createSection(skinsTab, 'Inventory Changer');
  createOnOffDropDown(inventorySection, "Enable Inventory Changer", 'combat', 'inventory_enable');
  separator(inventorySection);
  createDropDown(inventorySection, "AK-47", 'combat', 'inventory_ak_skin', ["Off", "Redline", "Vulcan", "Aquamarine Revenge", "Fuel Injector", "Bloodsport", "Neon Revolution", "The Empress", "Legion of Anubis", "Nightwish"]);
  createDropDown(inventorySection, "M4A1-S", 'combat', 'inventory_m4_skin', ["Off", "Hyper Beast", "Mecha Industries", "Player Two", "Printstream", "Nightmare"]);
  createDropDown(inventorySection, "AWP", 'combat', 'inventory_awp_skin', ["Off", "Asiimov", "Hyper Beast", "Containment Breach", "Neo-Noir", "Chrome Cannon", "Dragon Lore"]);
  createDropDown(inventorySection, "Desert Eagle", 'combat', 'inventory_deagle_skin', ["Off", "Blaze", "Conspiracy", "Kumicho Dragon", "Printstream", "Ocean Drive"]);
  createDropDown(inventorySection, "Knife", 'combat', 'inventory_knife_type', ["Karambit", "M9 Bayonet", "Bayonet", "Butterfly", "Talon", "Skeleton"]);
  createDropDown(inventorySection, "Knife Skin", 'combat', 'inventory_knife_skin', ["Off", "Doppler", "Marble Fade", "Tiger Tooth", "Fade", "Slaughter", "Crimson Web"]);
  createPaintKitTextEntry(inventorySection, "Wear 1-1000", 'skin_wear');
  createPaintKitTextEntry(inventorySection, "Seed", 'skin_seed');
  createYesNoDropDown(inventorySection, "StatTrak", 'combat', 'inventory_stattrak');
  separator(inventorySection);
  var applyContainer = $.CreatePanel('Panel', inventorySection, '', { class: "SettingsMenuDropdownContainer" });
  var applyBtn = $.CreatePanel('Button', applyContainer, 'skin_apply_button', {
    class: "PopupButton White",
    style: "horizontal-align: right; margin-right: 8px;",
    onactivate: "$.AimSync.addCommand('set', 'combat/skin_apply/1');"
  });
  $.CreatePanel('Label', applyBtn, '', { text: "Apply to Inventory" });

  $.AimSync.navigateToSubTab('combat', 'sniper_rifles');

  var hud = createTab('hud');
  
  var bomb = createSection(hud, 'Bomb Tracker');
  createYesNoDropDown(bomb, "Show Bomb Explosion Countdown And Site", 'hud', 'bomb_timer');
  separator(bomb);
  createYesNoDropDown(bomb, "Show Bomb Defuse Countdown", 'hud', 'defusing_alert');
  separator(bomb);
  createYesNoDropDown(bomb, "Show Bomb Plant Alert", 'hud', 'bomb_plant_alert');

  var killfeed = createSection(hud, 'Killfeed');
  separator(killfeed);
  createYesNoDropDown(killfeed, "Preserve My Killfeed During The Round", 'hud', 'preserve_killfeed');

  var time = createSection(hud, 'Time');
  separator(time);
  createYesNoDropDown(time, "Show Post-round Timer", 'hud', 'postround_timer');

  var visuals = createVisualsTab();

  var playerInfoTab = createSubTab(visuals, 'player_info');

  var playerInfo = createSection(playerInfoTab, 'Player ESP');
  createDropDown(playerInfo, "Master Switch", 'visuals', 'player_information_through_walls', ['Enemies', 'All Players', 'Off']);

  var playerPosition = createSection(playerInfoTab, 'Player Position');
  createYesNoDropDown(playerPosition, "Show Player Position Arrow", 'visuals', 'player_info_position');
  separator(playerPosition);
  createDropDown(playerPosition, "Player Position Arrow Color", 'visuals', 'player_info_position_color', ['Player / Team Color', 'Team Color']);

  var playerName = createSection(playerInfoTab, 'Player Name');
  createYesNoDropDown(playerName, "Show Player Name", 'visuals', 'player_info_name');
  separator(playerName);
  createDropDown(playerName, "Player Name Color", 'visuals', 'player_info_name_color', ['White', 'Team Color', 'Health-based']);

  var playerHealth = createSection(playerInfoTab, 'Player Health');
  createYesNoDropDown(playerHealth, "Show Player Health", 'visuals', 'player_info_health');
  separator(playerHealth);
  createDropDown(playerHealth, "Player Health Text Color", 'visuals', 'player_info_health_color', ['Health-based', 'White']);

  var playerWeapon = createSection(playerInfoTab, 'Player Weapon');
  createYesNoDropDown(playerWeapon, "Show Player Active Weapon Icon", 'visuals', 'player_info_weapon');
  separator(playerWeapon);
  createYesNoDropDown(playerWeapon, "Show Player Active Weapon Ammo", 'visuals', 'player_info_weapon_clip');
  separator(playerWeapon);
  createYesNoDropDown(playerWeapon, 'Show Bomb Carrier Icon', 'visuals', 'player_info_bomb_carrier');
  separator(playerWeapon);
  createYesNoDropDown(playerWeapon, 'Show Bomb Planting Icon', 'visuals', 'player_info_bomb_planting');

  var playerIcons = createSection(playerInfoTab, 'Icons');
  createYesNoDropDown(playerIcons, "Show Defuse Icon", 'visuals', 'player_info_defuse');
  separator(playerIcons);
  createYesNoDropDown(playerIcons, 'Show Picking Up Hostage Icon', 'visuals', 'player_info_hostage_pickup');
  separator(playerIcons);
  createYesNoDropDown(playerIcons, 'Show Rescuing Hostage Icon', 'visuals', 'player_info_hostage_rescue');
  separator(playerIcons);
  createYesNoDropDown(playerIcons, 'Show Blinded By Flashbang Icon', 'visuals', 'player_info_blinded');

  var outlineGlowTab = createSubTab(visuals, 'outline_glow');

  var outlineGlow = createSection(outlineGlowTab, 'Outline ESP');
  createOnOffDropDown(outlineGlow, "Master Switch", 'visuals', 'outline_glow_enable');

  var playerOutlineGlow = createSection(outlineGlowTab, 'Players');
  createDropDown(playerOutlineGlow, "Glow Players", 'visuals', 'player_outline_glow', ['Enemies', 'All Players', 'Off']);
  separator(playerOutlineGlow);
  createDropDown(playerOutlineGlow, "Player Glow Color", 'visuals', 'player_outline_glow_color', ['Player / Team Color', 'Team Color', 'Health-based', 'Enemy / Ally']);
  separator(playerOutlineGlow);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Player Blue Hue", 'player_outline_glow_blue_hue', 191, 240);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Player Green Hue", 'player_outline_glow_green_hue', 110, 140);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Player Yellow Hue", 'player_outline_glow_yellow_hue', 47, 60);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Player Orange Hue", 'player_outline_glow_orange_hue', 11, 20);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Player Purple Hue", 'player_outline_glow_purple_hue', 250, 280);
  separator(playerOutlineGlow);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Team T Hue", 'player_outline_glow_t_hue', 30, 40);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Team CT Hue", 'player_outline_glow_ct_hue', 210, 230);
  separator(playerOutlineGlow);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "High Health Hue", 'player_outline_glow_high_hp_hue', 0, 359);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Low Health Hue", 'player_outline_glow_low_hp_hue', 0, 359);
  separator(playerOutlineGlow);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Enemy Hue", 'player_outline_glow_enemy_hue', 0, 359);
  separator(playerOutlineGlow);
  createHueSlider(playerOutlineGlow, "Ally Hue", 'player_outline_glow_ally_hue', 0, 359);

  var weaponOutlineGlow = createSection(outlineGlowTab, 'Weapons');
  createYesNoDropDown(weaponOutlineGlow, "Glow Weapons on Ground Nearby", 'visuals', 'weapon_outline_glow');
  separator(weaponOutlineGlow);
  createYesNoDropDown(weaponOutlineGlow, "Glow Grenade Projectiles", 'visuals', 'grenade_proj_outline_glow');
  separator(weaponOutlineGlow);
  separator(weaponOutlineGlow);
  createHueSlider(weaponOutlineGlow, "Flashbang Hue", 'outline_glow_flashbang_hue', 191, 250);
  separator(weaponOutlineGlow);
  createHueSlider(weaponOutlineGlow, "HE Grenade Hue", 'outline_glow_hegrenade_hue', 300, 359);
  separator(weaponOutlineGlow);
  createHueSlider(weaponOutlineGlow, "Smoke Grenade Hue", 'outline_glow_smoke_hue', 110, 140);
  separator(weaponOutlineGlow);
  createHueSlider(weaponOutlineGlow, "Molotov / Incendiary Grenade Hue", 'outline_glow_molotov_hue', 20, 60);

  var bombAndDefuseKitOutlineGlow = createSection(outlineGlowTab, 'Bomb & Defuse Kit');
  createYesNoDropDown(bombAndDefuseKitOutlineGlow, "Glow Dropped Bomb", 'visuals', 'dropped_bomb_outline_glow');
  separator(bombAndDefuseKitOutlineGlow);
  createYesNoDropDown(bombAndDefuseKitOutlineGlow, "Glow Ticking Bomb", 'visuals', 'ticking_bomb_outline_glow');
  separator(bombAndDefuseKitOutlineGlow);
  createYesNoDropDown(bombAndDefuseKitOutlineGlow, "Glow Defuse Kits on Ground Nearby", 'visuals', 'defuse_kit_outline_glow');
  separator(bombAndDefuseKitOutlineGlow);
  separator(bombAndDefuseKitOutlineGlow);
  createHueSlider(bombAndDefuseKitOutlineGlow, "Dropped Bomb Hue", 'outline_glow_dropped_bomb_hue', 0, 359);
  separator(bombAndDefuseKitOutlineGlow);
  createHueSlider(bombAndDefuseKitOutlineGlow, "Ticking Bomb Hue", 'outline_glow_ticking_bomb_hue', 0, 359);
  separator(bombAndDefuseKitOutlineGlow);
  createHueSlider(bombAndDefuseKitOutlineGlow, "Defuse Kit Hue", 'outline_glow_defuse_kit_hue', 0, 359);

  var hostageOutlineGlow = createSection(outlineGlowTab, 'Hostages');
  createYesNoDropDown(hostageOutlineGlow, "Glow Hostages", 'visuals', 'hostage_outline_glow');
  separator(hostageOutlineGlow);
  createHueSlider(hostageOutlineGlow, "Hostage Hue", 'outline_glow_hostage_hue', 0, 359);

  var _modelGlowTab = createSubTab(visuals, 'model_glow');
  _modelGlowTab.style.overflow = 'squish squish';
  _modelGlowTab.style.flowChildren = 'right';

  var modelGlowPreview = $.CreatePanel('Panel', _modelGlowTab, '', { style: 'flow-children: down;' });
  $.CreatePanel('Label', modelGlowPreview, '', { style: 'vertical-align: top; horizontal-align: center; font-size: 40; color: ' + accentColor + '; font-weight: bold;', text: 'Preview' });
  var playerModelGlowPreview = $.CreatePanel('Panel', modelGlowPreview, '', { style: 'flow-children: right; margin-top: 20px;' });
  createPlayerModelGlowPreview(playerModelGlowPreview, 'ModelGlowPreviewPlayerTT', 'ModelGlowPreviewPlayerTTLabel', 'agents/models/tm_professional/tm_professional_varf.vmdl', makeFauxItemId(7, 921));
  createPlayerModelGlowPreview(playerModelGlowPreview, 'ModelGlowPreviewPlayerCT', 'ModelGlowPreviewPlayerCTLabel', 'agents/models/ctm_st6/ctm_st6_variante.vmdl', makeFauxItemId(9, 819));

  $.CreatePanel('Label', modelGlowPreview, '', { style: 'horizontal-align: center; margin-top: 20px;', text: 'Weapons on the Ground' });

  var weaponModelGlowPreview = $.CreatePanel('Panel', modelGlowPreview, '', { style: 'flow-children: right;' });

  var modelGlowPreviewWeapon = $.CreatePanel('MapItemPreviewPanel', weaponModelGlowPreview, 'ModelGlowPreviewWeapon', {
    map: "ui/xpshop_item",
    camera: "camera_weapon_0",
    "require-composition-layer": true,
    player: false,
    initial_entity: "item",
    mouse_rotate: false,
    sync_spawn_addons: true,
    "transparent-background": true,
    "pin-fov": "vertical",
    style: "width: 400px; height: 160px;"
  });
  modelGlowPreviewWeapon.SetItemItemId(makeFauxItemId(16, 255), {});

  var grenadeModelGlowPreview = $.CreatePanel('Panel', weaponModelGlowPreview, '', { style: 'flow-children: down;' });

  var grenadeModelGlowPreviewRow1 = $.CreatePanel('Panel', grenadeModelGlowPreview, '', { style: 'flow-children: right;' });
  createGrenadeModelGlowPreview(grenadeModelGlowPreviewRow1, 'ModelGlowPreviewFlashbang', 43);
  createGrenadeModelGlowPreview(grenadeModelGlowPreviewRow1, 'ModelGlowPreviewHEGrenade', 44);

  var grenadeModelGlowPreviewRow2 = $.CreatePanel('Panel', grenadeModelGlowPreview, '', { style: 'flow-children: right;' });
  createGrenadeModelGlowPreview(grenadeModelGlowPreviewRow2, 'ModelGlowPreviewSmoke', 45);
  createGrenadeModelGlowPreview(grenadeModelGlowPreviewRow2, 'ModelGlowPreviewIncendiary', 48);

  var modelGlowTab = $.CreatePanel('Panel', _modelGlowTab, '', { style: 'flow-children: down; margin-right: 40px; overflow: squish scroll;' });

  var modelGlow = createSection(modelGlowTab, 'Model ESP');
  createOnOffDropDown(modelGlow, "Master Switch", 'visuals', 'model_glow_enable');

  var playerModelGlow = createSection(modelGlowTab, 'Players');
  createDropDown(playerModelGlow, "Glow Player Models", 'visuals', 'player_model_glow', ['Enemies', 'All Players', 'Off']);
  separator(playerModelGlow);
  createDropDown(playerModelGlow, "Player Model Glow Color Mode", 'visuals', 'player_model_glow_color', ['Player / Team Color', 'Team Color', 'Health-based', 'Enemy / Ally']);
  separator(playerModelGlow);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Player Blue Hue", 'player_model_glow_blue_hue', 191, 240);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Player Green Hue", 'player_model_glow_green_hue', 110, 140);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Player Yellow Hue", 'player_model_glow_yellow_hue', 47, 60);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Player Orange Hue", 'player_model_glow_orange_hue', 11, 20);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Player Purple Hue", 'player_model_glow_purple_hue', 250, 280);
  separator(playerModelGlow);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Team T Hue", 'player_model_glow_t_hue', 30, 40);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Team CT Hue", 'player_model_glow_ct_hue', 210, 230);
  separator(playerModelGlow);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "High Health Hue", 'player_model_glow_high_hp_hue', 0, 359);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Low Health Hue", 'player_model_glow_low_hp_hue', 0, 359);
  separator(playerModelGlow);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Enemy Hue", 'player_model_glow_enemy_hue', 0, 359);
  separator(playerModelGlow);
  createHueSlider(playerModelGlow, "Ally Hue", 'player_model_glow_ally_hue', 0, 359);
)"
// split the string literal because MSVC does not support string literals longer than 16k chars - error C2026
u8R"(
  var weaponModelGlow = createSection(modelGlowTab, 'Weapons');
  createYesNoDropDown(weaponModelGlow, "Glow Weapon Models on Ground", 'visuals', 'weapon_model_glow');
  separator(weaponModelGlow);
  createYesNoDropDown(weaponModelGlow, "Glow Grenade Projectile Models", 'visuals', 'grenade_proj_model_glow');
  separator(weaponModelGlow);
  separator(weaponModelGlow);
  createHueSlider(weaponModelGlow, "Flashbang Hue", 'model_glow_flashbang_hue', 191, 250);
  separator(weaponModelGlow);
  createHueSlider(weaponModelGlow, "HE Grenade Hue", 'model_glow_hegrenade_hue', 300, 359);
  separator(weaponModelGlow);
  createHueSlider(weaponModelGlow, "Smoke Grenade Hue", 'model_glow_smoke_hue', 110, 140);
  separator(weaponModelGlow);
  createHueSlider(weaponModelGlow, "Molotov / Incendiary Grenade Hue", 'model_glow_molotov_hue', 20, 60);

  var bombModelGlow = createSection(modelGlowTab, 'Bomb & Defuse Kit');
  createYesNoDropDown(bombModelGlow, "Glow Dropped Bomb Model", 'visuals', 'dropped_bomb_model_glow');
  separator(bombModelGlow);
  createYesNoDropDown(bombModelGlow, "Glow Ticking Bomb Model", 'visuals', 'ticking_bomb_model_glow');
  separator(bombModelGlow);
  createYesNoDropDown(bombModelGlow, "Glow Defuse Kit Models on Ground", 'visuals', 'defuse_kit_model_glow');
  separator(bombModelGlow);
  separator(bombModelGlow);
  createHueSlider(bombModelGlow, "Dropped Bomb Hue", 'model_glow_dropped_bomb_hue', 0, 359);
  separator(bombModelGlow);
  createHueSlider(bombModelGlow, "Ticking Bomb Hue", 'model_glow_ticking_bomb_hue', 0, 359);
  separator(bombModelGlow);
  createHueSlider(bombModelGlow, "Defuse Kit Hue", 'model_glow_defuse_kit_hue', 0, 359);

  var _viewmodelTab = createSubTab(visuals, 'viewmodel');
  _viewmodelTab.style.overflow = 'squish squish';
  _viewmodelTab.style.flowChildren = 'right';

  var viewmodelPreviewContainer = $.CreatePanel('Panel', _viewmodelTab, '', { style: 'flow-children: down;' });
  $.CreatePanel('Label', viewmodelPreviewContainer, '', { style: 'vertical-align: top; horizontal-align: center; font-size: 40; color: ' + accentColor + '; font-weight: bold;', text: 'Preview' });

  var viewmodelPreview = $.CreatePanel('MapItemPreviewPanel', viewmodelPreviewContainer, 'ViewmodelPreview', {
    map: "ui/xpshop_item",
    camera: "camera_weapon_0",
    "require-composition-layer": true,
    player: false,
    initial_entity: "item",
    mouse_rotate: false,
    sync_spawn_addons: true,
    "transparent-background": true,
    "pin-fov": "vertical",
    style: "width: 700px; height: 400px;"
  });
  viewmodelPreview.SetHideStaticGeometry(true);

  var viewmodelTab = $.CreatePanel('Panel', _viewmodelTab, '', { style: 'flow-children: down; margin-right: 40px; overflow: squish scroll;' });

  var viewmodelModification = createSection(viewmodelTab, 'Viewmodel Modification');
  createOnOffDropDown(viewmodelModification, "Master Switch", 'visuals', 'viewmodel_mod');

  var viewmodelFov = createSection(viewmodelTab, 'Viewmodel Fov');
  createYesNoDropDown(viewmodelFov, "Modify Viewmodel Fov", 'visuals', 'viewmodel_fov_mod');
  separator(viewmodelFov);
  createSlider(viewmodelFov, "Fov", 'viewmodel_fov', 40, 90);

  var _grenadePredictionTab = createSubTab(visuals, 'grenade_prediction');

  var grenadePredictionMaster = createSection(_grenadePredictionTab, 'Grenade Prediction');
  createOnOffDropDown(grenadePredictionMaster, "Master Switch", 'visuals', 'grenade_prediction_enable');
  separator(grenadePredictionMaster);
  createHueSlider(grenadePredictionMaster, "Trajectory Color", 'grenade_prediction_trajectory_hue', 0, 359);
  separator(grenadePredictionMaster);
  createHueSlider(grenadePredictionMaster, "Bounce Color", 'grenade_prediction_bounce_hue', 0, 359);
  separator(grenadePredictionMaster);
  createSlider(grenadePredictionMaster, 'Bounce Friction (0 = none, 200 = 0.200)', 'grenade_prediction_bounce_friction', 0, 200);

  var sound = createTab('sound');
  
  var playerSoundVisualization = createSection(sound, 'Footsteps');
  separator(playerSoundVisualization);
  createYesNoDropDown(playerSoundVisualization, "Visualize Player Footstep Sound", 'sound', 'visualize_player_footsteps');

  var bombSoundVisualization = createSection(sound, 'Bomb Audio');
  createYesNoDropDown(bombSoundVisualization, "Visualize Bomb Plant Sound", 'sound', 'visualize_bomb_plant');
  separator(bombSoundVisualization);
  createYesNoDropDown(bombSoundVisualization, "Visualize Bomb Beep Sound", 'sound', 'visualize_bomb_beep');
  separator(bombSoundVisualization);
  createYesNoDropDown(bombSoundVisualization, "Visualize Bomb Defuse Sound", 'sound', 'visualize_bomb_defuse');

  var weaponSoundVisualization = createSection(sound, 'Weapon Audio');
  createYesNoDropDown(weaponSoundVisualization, "Visualize Weapon Scope Sound", 'sound', 'visualize_scope_sound');
  separator(weaponSoundVisualization);
  createYesNoDropDown(weaponSoundVisualization, "Visualize Weapon Reload Sound", 'sound', 'visualize_reload_sound');

  $.AimSync.navigateToTab('combat');
})();
)"
