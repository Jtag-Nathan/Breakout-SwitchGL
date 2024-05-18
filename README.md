# Breakout-SwitchGL

Learn OpenGL Breakout game for Nintendo Switch.

Made using devkitPro, libnx, OpenGL, GLFW, GLAD, STB_IMAGE, IMGUI(Docking)

requires the devkitPro toolchain and libnx to be installed/available to build.

Edit the NxLink.bat file with your Nintendo Switch IP

Edit the Makefile if you want to change the Icon or the App/Author name etc.

## BUILD INSTRUCTIONS

Run these commands one after the other.

First clone the repo.

```bash
git clone https://github.com/Jtag-Nathan/Breakout-SwitchGL.git
```

Second cd into the downloaded dir.

```bash
cd Breakout-SwitchGL
```

Now run make to build the game.

```bash
make
```

Finally run the NxLink.bat to upload the game to the Nintendo Switch.

```bash
NxLink.bat
```
