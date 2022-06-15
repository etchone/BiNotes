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
        component.addOperation("CreateShortcut", "@TargetDir@/BiNotes.exe", "@DesktopDir@/BiNotes.lnk");
    }
}