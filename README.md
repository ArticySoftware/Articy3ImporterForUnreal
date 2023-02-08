<p align="center">
  <img height="344" src="https://www.articy.com/articy-importer/unreal/ad_importer_large.png">
</p>

# ArticyImporter for Unreal Engine - V 1.6.1

Fast path branch

*  Importer version that can generates code side by side with game code (as usual) OR inside an Unreal generated plugin, to avoid game code recompilation every time the importer regenerate code.
* [TD] Migration manager to be able to switch from game code to generated plugin and back...

## Fast path branch :
- Reroute code generation inside a "ArticyGenerated" module.
- Generated module uses code from Unreal to automatically generate a module.
- Trigger recompilation / hot reload of only the "ArticyGenerated" module (no other modules files should be unneeded recompiled).

### Raising Problems to adress before merge :
Will need engine restart if user sets different import path method
TODO => inform developer + lock any import until restart

Will break existing blueprints
TODO => Migration manager to generate core redirectors in config files

May crash unreal if the developer uses a different compilation experience than the plugin (Hotreload vs live coding race conditions)
TODO => Detect developer compilation experience and trigger compilation accordingly, or just let Unreal handle that ?

Despite severe efforts, the current version is not already compatible with unreal 5.1.x (remains UE 5.0.1 only).
