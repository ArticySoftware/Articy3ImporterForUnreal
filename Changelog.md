## Unreal Importer 1.0.2 Changelog
- New Features:
    - Articy Flow Debugger added
        - The flow debugger is an actor found in the plugin content folder (not the generated ArticyContent folder!), which can be placed in the world.
        Upon setting the 'Start On' articy reference to a flow object of your choice and hitting Play, a simple UI will popup to display your dialogue and dialogue branches.
        Depending on the 'Ignore invalid branches' bool, branches with unfulfilled conditions will either not appear or they will show up in red.
        This is a means to test your imported dialogue easily without needing to setup a UI on your own.

- General:
    - Changed: Articy Flow Player's 'Start On' attribute now can only select objects in the ArticyNode hierarchy (flow objects effectively, rather than entities)
    - Changed: Removal of several monolithic headers (Engine.h and SlateBasics.h) and many include changes across the board
    - Fix: ExpressoScripts that compare integers with floats now behave correctly. This is valid for all comparison operators (<, >, <=, >=, ==, !=)
    - Fix: Compilation errors for Mac, Linux, and iOS.
    
## Unreal Importer 1.0.1 Changelog
- New Features:
    - ArticyRef meta data attribute "ArticyClassRestriction" added in C++
        This meta data attribute will set the class filter restriction to the chosen class permanently and can not be changed without changing the meta data.
        This allows programmers to set the allowed class hierarchy for a Blueprint-exposed ArticyRef structure.
        Example here:
        ```
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup", meta=(ArticyClassRestriction="ArticyNode"))
	    FArticyRef StartOn;
        ```

- Blueprint:
    - "Should pause on" function of the Articy Flow Player exposed to Blueprints.
    This function allows you to test whether the flow player pauses on an articy node or not.
    - "Get Type" function of the Articy Node classes function exposed to Blueprints.
    This function allows you get the type of a generic ArticyNode (Flow Fragment, Dialog Fragment etc.) and can be used in a Switch node.

- C++:
    - Added export macros to the generated UCLASSES and USTRUCTS.

- General:
    - Fixes in the backend to compile as an engine plugin

## Unreal Importer 1.0.0 Changelog

- Disclaimer: Please perform a 'full reimport' after upgrading to this version of the importer by opening up the new Articy Importer window in the level toolbar and clicking 'Force complete reimport'
In case error messages pop up, please close Unreal, recompile the project inside Visual Studio and start up the engine again.

- Articy Importer window added
    - This window hosts the main controls of the importer. The button to open the window can be found in the level toolbar. The window will be expanded in the future with more options and functionality. As a consequence, the import options inside the plugin settings and the import data assets have been removed. Currently it enables the user to perform three import actions:
        - Force complete reimport
        - Reimport changes
        - Regenerate assets

- Import Cache & Restoration added
    - The importer will now cache the last valid import state and will try to restore that state when a new import fails to compile.

- Blueprint:
    - Changed: ImportedPackages map of the Import Data Asset is no longer blueprint readable

- C++:
    - Changed: ArticyImporter module renamed to ArticyEditor
    - Changed: The Articy Asset Picker is now exported to other modules, meaning that it can be accessed for custom purposes without modifying plugin code

- General:
    - Stability improved
    - Added: Editor resources to better represent articy:draft related functionality
    - Changed: PIE import queue now uses 'Reimport changes' instead of 'Complete reimport'
    - Changed: Folder structure of the plugin. Code depending on paths, such as includes, may need to adapt to the new structure.
    - Fix: Importing after closing the plugin settings no longer crashes the engine

## Unreal Importer 0.0.5 Changelog

- Disclaimer: Please perform a 'full reimport' after upgrading to this version of the importer by going into the plugin settings and clicking 'Force complete reimport'
In case error messages pop up, please close Unreal, recompile the project inside Visual Studio and start up the engine again.
This is due to Unreal's Hot Reload not handling changes in header files well. Due to class hierarchies changing this can lead to temporary error messages.

- Articy Asset Picker added for ArticyRef variables
    - The new asset picker enables an easy lookup and selection of imported articy objects
    - Tooltips provide more information on the various objects
    - Double clicking the image of an ArticyRef variable opens up the selected asset inside Unreal
    - The articy button next to the image opens up the selected object in articy:draft
    - When expanding the ArticyRef variable, a class filter can be set to allow the asset picker to only show select objects (dialogue fragments, entities etc.)
    - A search filter lets you browse through the available objects. You can filter by class name, ID, text, speakers and various other attributes

- One Unreal asset per package
    - An exported package in articy:draft now generates one asset inside Unreal with all specified data inside.
    This allows for much faster reimports as less assets have to be handled when reimporting

- Import optimization:
    - When hitting the Import button, a compilation process now only happens when script fragments, template definitions or global variables changed.
    If none of these changed, the import process finishes almost immediately

- Blueprint:
    - Added: GetOutputPins function for all classes that implement the OutputPinsProvider interface (e.g. all flow objects)
    - Added: ArticyPin: GetOwner function is now BlueprintCallable
    - Added: GlobalVariables: GetVariablesOfType function added in which you can specify what type of variables you want (all, Ints, Strings or Bools)
    - Added: ArticyAsset: LoadAsset function
    - Changed: Target & TargetPin of ArticyJump are now BlueprintReadOnly instead of BlueprintReadWrite

- C++:
    - Added: UArticyJump: GetTargetID and GetTargetPinID functions (same access rules as in Blueprints)
    - Added: UArticyObject: GetArticyObjectChildrenIDs function (returns all children IDs that represent ArticyObjects)
    - Added: UArticyObject: GetParentID and GetChildrenIDs() functions
    - Changed: FindAsset function moved from UArticyPrimitive to UArticyObject
    - Changed: Renamed GetOutputPins function to GetOutputPinsPtr
    - Changed: All headers were moved to the Public folder, all cpp files were moved to the Private folder

- General:
    - UE 4.19 is no longer supported starting with this release. Please use the dedicated UE 4.19 plugin instead
    - Added: If Perforce is used for source control, the generated code is now automatically checked out
    - Added: Console Command: "ArticyImporter.Reimport" works as if hitting the import button when prompted (adaptive reimport)
    - Changed: ArticyRef helper functions now take UArticyObjects as parameters instead of UArticyPrimitives
    - Changed: Various generated classes now inherit from more fitting classes rather than from ArticyObject ({YourProject}Entity now inherits from ArticyEntity instead of ArticyObject)
    - Changed: Generated articy assets in Unreal will now save automatically after being generated
    - Fixed: Removing a GlobalVariable set and reimporting no longer results in uncompileable code
    - Fixed: GlobalVariables: SetByNamespaceAndVariable function now works as intended (parameters were used in the wrong order internally)
    - Fixed: Setting a non-existing global variable inside an existing variable set no longer leads to a crash
