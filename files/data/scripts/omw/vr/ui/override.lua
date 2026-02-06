local I = require('openmw.interfaces')
local util = require('openmw.util')

local windowToLayer = {
    Alchemy = 'Windows',
    Book = 'JournalBooks',
    Companion = 'InventoryCompanionWindow',
    Container = 'InventoryCompanionWindow',
    Dialogue = 'DialogueWindow',
    EnchantingDialog = 'Windows',
    Inventory = 'InventoryWindow',
    JailScreen = 'Windows',
    Journal = 'JournalBooks',
    LevelUpDialog = 'Windows',
    Magic = 'SpellWindow',
    Map = 'MapWindow',
    MerchantRepair = 'Windows',
    QuickKeys = 'Windows',
    Recharge = 'Windows',
    Repair = 'Windows',
    Scroll = 'JournalBooks',
    SpellBuying = 'Windows',
    SpellCreationDialog = 'Windows',
    Stats = 'StatsWindow',
    Trade = 'InventoryCompanionWindow',
    Training = 'Windows',
    Travel = 'Windows',
    WaitDialog = 'Windows',
}

local baseInterface = I.UI
local overriddenInterface = {}
for k, v in pairs(baseInterface) do 
    overriddenInterface[k] = v 
end
overriddenInterface.windowToLayer = util.makeStrictReadOnly(windowToLayer)

return {
    interfaceName = 'UI',
    interface = overriddenInterface,
}
