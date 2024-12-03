# Sequence Unreal SDK

This SDK allows easy integration of Sequence Embedded Wallet from any Unreal Framework project.

[![Docs](https://img.shields.io/badge/Documentation-7334f8)](https://docs.sequence.xyz/sdk/unreal/introduction)
[![Boilerplate](https://img.shields.io/badge/Get%20started%20using%20our%20Boilerplate%20Project-7334f8)](https://docs.sequence.xyz/sdk/unreal/introduction)

## Requirements

Unreal Engine 5.2, 5.3, or 5.4

## Supported Platforms

- PC Standalone
- Mac Standalone
- Android
- iOS

## Samples



## TroubleShooting

In events where the editor crashes on launch, try deleting the following file, Saved/SaveGames/Cr.sav
This file contains the savegame data for credentials and if modified can result in an engine crash.
Deleting this file deletes local credentials, so you'll need to log back in.
