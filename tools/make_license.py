"""Builds LICENSE.txt from Hunterborn 2026's (same MIT text, GPL notice for the "new" DLL and library notices), with this project's own header and credits.

  python tools/make_license.py
"""
import os

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
src = open(r"E:/WorkSpace/Hunterborn-2026/LICENSE.txt", encoding="utf-8", newline="").read().replace(chr(13) + chr(10), chr(10))
# everything from the MIT text on is shared; the header (title, scope, credits) is ours
start = src.index("Permission is hereby granted")
body = src[start:]
header = """Hunting in Skyrim 2026
Copyright (c) 2026 CageTV

Released under the MIT License. Source code: https://github.com/CageTV/Hunting-in-Skyrim-2026

This license covers the files written for this repository (HuntingInSkyrimNative.dll and its settings pages). It does not cover plugin/include/
SKSEMenuFramework.h (the SKSE Menu Framework 3 project's SDK header, under that project's terms).

CREDITS - the work this builds on, used with its author's permission and credited here and on the download page
  Hunting in Skyrim - A Hunting Guild SE and Fishing in Skyrim, by Richard Webster (B1gBadDaddy), who has given permission to update them.
    A REQUIREMENT, not part of this download: its plugin, scripts, meshes, textures and sounds are not included. This plugin only calls its
    quests and reads its forms while the game runs.
  The original author keeps the copyright in the work; it is used under the permission given, and it is not re-licensed.

The files written for this repository are released under the MIT License below, and the "new" HuntingInSkyrimNative.dll is also under the GNU
General Public License as described at the end of this file.

"""
text = (header + body).replace("HunterbornNative.dll", "HuntingInSkyrimNative.dll").replace("Hunterborn 2026", "Hunting in Skyrim 2026").replace(
    "Hunterborn-2026", "Hunting-in-Skyrim-2026")
open(os.path.join(HERE, "LICENSE.txt"), "w", encoding="utf-8", newline="\n").write(text)
print("LICENSE.txt:", len(text), "chars")
