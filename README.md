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

On Debian, Ubuntu and distributions based the on two,

```
sudo apt install libwxbase3.0-dev libwxgtk-media3.0-gtk3-dev libwxgtk3.0-gtk3-dev wx3.0-headers libwxsvg-dev libwxsvg3 libsqlite3-dev libyaml-cpp-dev libtagc0-dev libtag1-dev libtagc0 libexif-dev libpango1.0-dev
```

You might also need to install `git`, `meson` and `g++` as well, if you don't already have them installed in order to build SampleHive.

*NOTE:* On Debian and Debian based distributions you also have to install `libwxgtk-media3.0-dev`

## How to build SampleHive?

Download the source code from this repository or use a git clone:

```
git clone https://gitlab.com/samplehive/sample-hive
cd sample-hive
meson build -Dprefix=/tmp/SampleHive
ninja -C build install
```

This will install SampleHive under `/tmp/SampleHive`.

The configuration file will be placed under `~/.config/SampleHive/config.yaml` and the database file will be placed under `~/.local/share/SampleHive/sample.hive`

*NOTE:* If you don't provide the `-Dprefix=/tmp/SampleHive` by default it will be installed under `/usr/local`. You can set the prefix to anything you want.

## How to run SampleHive?

To run SampleHive:

If you provided a prefix, you can go the prefix directory then go to the `bin` directory and run the SampleHive binary, for example, assuming the prefix was set to `/tmp/SampleHive`

```
cd /tmp/SampleHive/bin
./SampleHive
```

If you didn't provide a prefix, you can find SampleHive in your menu system or run launcher and run SampleHive as you would run any other program on you system.

## Are there any keybindings for SampleHive?

// TODO

## Can I configure SampleHive?

SampleHive comes with a `config.yaml` file, that is placed under `~/.config/SampleHive/config.yaml`, that you can edit to change some settings for it.
