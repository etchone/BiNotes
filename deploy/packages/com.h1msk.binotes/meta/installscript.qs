function Component()
{
}
Component.prototype.isDefault = function()
{
    return true;
}
Component.prototype.createOperations = function()
{
    try
    {
        component.createOperations();
    }
    catch (e)
    {
        print(e);
    }
    if (systemInfo.productType === "windows")
    {
        component.addOperation("CreateDesktopShortcut", "@TargetDir@/BiNotes.exe", "@DesktopDir@/BiNotes.lnk");
        component.addOperation("CreateStartMenuShortcut", "@TargetDir@/BiNotes.exe", "@StartMenuDir@/BiNotes.lnk");
        component.addOperation("CreateStartMenuShortcut", "@TargetDir@/maintenancetool.exe", "@StartMenuDir@/Uninstall BiNotes");
    }
}