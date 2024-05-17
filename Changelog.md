## Unreal Importer 1.8.0 Changelog :

- New Features: Unreal 5.4 compatibility

## Unreal Importer 1.7.1 Changelog :

- Fixes:
	- Fixed issue with literal strings
	- Fixed header issues on Unreal 5.3

## Unreal Importer 1.7.0 Changelog :

- New Features: Unreal 5.2 & 5.3 compatibility

## Unreal Importer 1.6.0 Changelog : 

- New Features: Unreal 5.1 compatibility

## Unreal Importer 1.5.1 Changelog : 

- New Features:
	- Slots evaluation for ExpressoScripts
- Changes:
- Fixes:
- Known issues (Should be fixed in 1.6): 
	- Expresso Print("A simple non-evaluated string..."); method won't work when used inside a branch where nodes also uses GetObj/SetObj/GetProperty/SetProperty
- Documentation (web)

## Unreal Importer 1.5 Changelog

- New Features:
	- Added a message box that verify for an "ArticyRuntime" reference in the dependency modules of the unreal build tool and ask the user to add it if it's not present.
	- Added a checkbox inside the Articy Importer plugin preferences to avoid automatic ArticyRuntime verification (case of the developper have a custom build pipeline).	
	- String representation : New node "Get Object from String Representation" that takes a string parameter (StringID CloneID) to get an object by its internal Articy String representation. 
	- New expresso methods (IncrementProp, DecrementProp, IsPropInRange, IsInRange) to reflect Articy:Draft Expresso new methods.
- Changes:
	- Removed "BranchLimit" from FlowPlayer
- Fixes:
	- Fixed generated files for PS5 and Androïd
	- Fixed Articy icon disappears when usign small editor icons for UE4
	- Fixed CloneID that was always "1"
- Documentation (web)
	- Updated "Getting the object" section to add String representation example.
	- Removed step by step import process section
	- Changed "Adjust build configuration" section of the documentation to reflect automatic ArticyRuntime reference new functionality.

## Unreal Importer 1.4 Changelog

- Unreal Engine 5 support
- Breaking Changes:
  - A change in the code generator will break existing projects. Search "Error C2451" in [the readme](README.md) for the quick fix to get you compiling again.
- New Features:
  - Rich text support using Unreal's Rich Text Block component, including hyperlinks ([#64](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/64)).
  - Support for multiple, independent global variable sets ([#66](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/66)).
- Changes:
  - Import with enabled live coding is now allowed in UE5.
  - Moved generated method `U<ProjectName>ExpressoScripts::GetUserMethodsProviderObject()` to `UArticyExpressoScripts::GetUserMethodsProviderObject()`.
- Fixes:
  - Fixed compilation issues with UE4.22
  - Fixed compilation issues with Linux cross-compilation toolchain v19
  - Fixed packaging compiler warning concerning `FADIHierarchy::RootObject` ([#60](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/60)).
  - Fixed unsupported pragma compile error when building for PlayStation 4/5 ([#59](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/59) and [#67](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/67)).
  - Fixed issue with Unicode characters in generated scripts.
  - Fixed crash when changing levels while using a custom script methods ([#63](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/63)).
  - Fixed issues with `GetWorld()` in `UArticyDatabase` and `UArticyBaseObject` ([#68](https://github.com/ArticySoftware/ArticyImporterForUnreal/issues/68)).
- Documentation
  - Added documentation for custom script methods and shadowing to the Readme ([#61](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/61)).

## Unreal Importer 1.3.1 Changelog

- General:
    - New warning dialogue if you try to run the importer and a hotload is required while Live Coding is enabled ([#53](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/53)).
- New Features:
    - Support for the new `Matrix` property for Location objects like Zones, Images, Text, etc. ([#56](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/56))
    - Allow `setProp` Expresso function to be used to set Reference Slots using `getObj` ([#51](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/51)).
    - Expose `GetInputPins` to Blueprint. Note that this involves a breaking change for anyone previously using `GetInputPins()` in C++. Please change all usages to `GetInputPinsPtr()`. ([#58](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/58))
- Fixes:
    - Show an error message instead of crashing when failing to regenerate assets ([#48](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/48)).
    - PausedOn nodes are no longer executed twice (once on arrival, once on leaving). This only has an effect if you're pausing on Instruction nodes ([#52](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/52)).
    - No longer generate uncompilable code while using Blueprint Nativize ([#55](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/55)).
    - Added proper dependencies so `ArticyEditorModule` can be included in another module without errors ([#57](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/57)).


## Unreal Importer 1.3.0 Changelog
- Unreal Engine 5 Early Access 2 Support ([#41](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/41)).

- Fixes:
    - Fixed issue with plugin marking database and package assets for delete. Fixed in [#42](https://github.com/ArticySoftware/ArticyImporterForUnreal/pull/42). Resolves [#39](https://github.com/ArticySoftware/ArticyImporterForUnreal/issues/39).
    - Support for Unreal 4.27

## Unreal Importer 1.2.0 Changelog
- Unreal Engine 4.26 Support
- Unreal Engine 4.20 and 4.21 no longer supported
- New Features:
	- Custom Articy functions in expresso scripts now work properly with "self" and "GetObj" as parameters. The functions will use ArticyPrimitive as parameter. Keep in mind that "self" will give you a pin if used from inside a pin's expression. Cast to ArticyFlowPin and then call 'Get Owner' to access the node the pin is called on.
	- Articy directory location and import asset location can now be changed! The previous hierarchy needs to be maintained. Either move your existing assets to the new location and change the "Articy Directory" in the plugin settings to the parent folder of the import asset (previously would be the Content folder), or make sure to delete all pre-existing articy assets (.articyue4 file, import asset, generated assets, assets such as images) and do a fresh import from articy to the new location.
	
- General:
	- Added: Category "Articy Methods Provider" for articy custom functions
	- Added: Support for ArticyRef/Id widget blueprint pins for Articy Function Library functions (Get Object on an ArticyRef for example)
	- Fixed: Plugin Settings for package loading now refreshes upon asset regeneration rather than import.
	- Fixed: Articy Object tooltips now display the Articy Id even without generated articy assets (before, you didn't know if the Id was set or not if the object didn't exist)
	- Breaking Change: EArticyPausableType enum spelling for Dialog (-> Dialogue) and DialogFragment (-> DialogueFragment).
	
- C++:
	- Added: SetPauseOn function for ArticyFlowPlayer that can take a bit-masked value to support multiple types at once
	- Added: Automatic cleanup of your Articy Id Widget customization factories. While you can keep a reference to your factories yourself, you don't need to. The Articy Customization Manager will automatically clean up all factories that are registered at the point of shutdown.
	- Fixed: Templated GetObjectOfClass function now contains the objects with the specified clone id, if available, rather than the base object
	- Breaking Change: Articy Database now returns ArticyObjects rather than ArticyPrimitives (which were cast to ArticyObject in Blueprints automatically).
	
	

## Unreal Importer 1.1.0 Changelog
- New Features:
	- Articy Global Variables Debugger added to the articy toolbar
	- ArticyIdProperty Widget Customization system. Lets you add widgets from C++ to any SArticyIdProperty widget (ArticyId and ArticyRef structs primarily) without modifying plugin code
	- Custom widgets for ArticRef and ArticyId Blueprint pins
	- New ArticyRef widget supports Clone settings
	- ArticyIds use the previous ArticyRef widget
	- New C++ meta specifiers for ArticyRef and ArticyId types:
		- ArticyExactClass (locks the class filter if set to true)
		- ArticyNoWidget (only for ArticyId, removes the customized widget)
		
- General:
	- Added: Copy & Paste support for ArticyRef & ArticyId. ArticyRef copies can get pasted into ArticyIds and vice versa.
	- Added: Global Variables asset uses the same view as the new GV debugger. This fixes categorization issues and allows for search by namespace and variablename.
	- Added: Option in the plugin settings to sort children upon import. Default off as it degrades import performance.
	- Changed: Revamp of articy asset picker: now includes the class filter button and an 'Exact Class' filter checkbox
	- Changed: Articy asset picker now will always have its initial class restriction set to the highest possible in the hierarchy. Meaning: Blueprint created ArticyIds and ArticyRefs will display ArticyObject when opening the asset picker, C++-created ArticyIds and ArticyRefs with an "ArticyClassRestriction=..." meta specifier will have that class as the starting point.
	- Changed: The class filter in the Articy asset picker now uses a list rather than a tree structure
	- Changed: Articy Button on ArticyID/ArticyRef widgets now uses the current tab for ArticyNode elements (dialogues etc.) and opens up a new tab for entities instead. No more new windows!
	- Fix: Articy Import Data now constructs its hierarchy objects properly
	- Fix: Crash when selecting two actors of the same type with the same ArticyRef variables
	
- Blueprints:
	- Added: ArticyRef is now hashable and can be used in sets and maps as keys. You can add duplicates at the moment, which will get removed upon Blueprint compilation, rather than the default behavior of not letting you add duplicates in the first place. This lets you easily tweak the data structures. This might change in the future. See below in the C++ section for a more detailed explanation.
	- Added: MatchesRaw and MatchesEffective comparison functions for comparison of ArticyRefs. See below in the C++ section for a more detailed explanation.
	
- C++:
	- Added: Static UArticyImportData::GetImportData() function
	- Added: OnAssetsGenerated delegatein FArticyEditorModule, called whenever assets are generated. Previous "OnImportFinished" would not get called upon asset regeneration only.
	- Added: Static GetPackagesSlow() function in FArticyEditorModule
	- Added: GetExpression function for ArticyScriptFragments, returning a script as a const FString reference
	- Added: Different ToString functions for FArtiyId and FArticyRef types
	- Added: Made FArticyRef hashable (combination of underlying ID + effective CloneId is used). Since hash containers in UE4 make use of the == operator the effective CloneID is compared rather than the actual CloneID (bReferenceBaseObject = true implies effective CloneId = 0, but the actual CloneId value can be different)
	- Added: New comparison functions for FArticyRef: MatchesRaw and MatchesEffective.
	- Added: FArticyId InitFromString function. Relies on the string contents to include a "Low=XXX" and "High=YYY" section.
	- Changed: UArticyObject::FindAsset() now is an editor-only function
	- Changed: UArticyObject::FindAsset() uses caching to avoid module and asset registry lookup. This improves performance significantly and ensures functionality for large articy projects inside UE4.	

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
