<p align="center">
    <img src="assets/icons/icon-hive_200x200.png" alt="sample-hive-icon" width="200" height="200">
</p>
<h1 align="center">SampleHive</h1>
<p align="center">
  A simple, modern audio sample browser/manager for GNU/Linux.
  <img src="assets/logo/logo-hive_1920x1080.png" alt="sample-hive-logo">
  <hr>
</p>

## What is SampleHive?

SampleHive let's you manage your audio samples in a nice and simple way, just add a directory where you store all your samples, or drag and drop a directory on it to add samples to it, and it will help sort, search, play and view some information about the sample. You can also drag and drop from SampleHive to other applications.

![Screenshot of SampleHive](assets/screenshots/screenshot-hive.png)

## Dependencies
On Arch based distributions,

```
sudo pacman -S wxgtk3 wxsvg sqlite taglib yaml-cpp
```

On Ubuntu and Ubuntu based distributions,

```
sudo apt install libwxbase3.0-dev libwxgtk-media3.0-gtk3-dev libwxgtk3.0-gtk3-dev wx3.0-headers libwxsvg-dev libwxsvg3 libsqlite3-dev libyaml-cpp-dev libtagc0-dev libtag1-dev libtagc0 libexif-dev
```

You might also need to install `git`, `meson` and `g++` as well, if you don't already have them installed in order to compile SampleHive.

## How to build SampleHive?

Download the source code from this repository or use a git clone:

```
git clone https://gitlab.com/apoorv569/sample-hive
cd sample-hive
meson build
ninja -C build
```

## How to run SampleHive?

To run SampleHive:

```
cd build
./SampleHive
```

## Are there any keybindings for SampleHive?

There are few pre-defined keybindings for SampleHive that you can use, such as

| KEYBIND | ACTION                   |
|---------+--------------------------|
| P       | Play the selected sample |
| L       | Toggle loop on/off       |
| M       | Toggle mute on/off       |
| S       | Stop the playing sample  |
| O       | Open the settings dialog |

## Can I configure SampleHive?

SampleHive comes with a `config.yaml` file, that you can edit to change some settings for it.
