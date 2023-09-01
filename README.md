Sequence Unreal Plugin
======================

This plugin comes with everything needed to get connected and send transactions via web3.

Note: all files and folders talked about in this readme will be found in the following directories with in your project folder
/Plugins/SequencePlugin/Source/SequencePlugin/Private 
or
/Plugins/SequencePlugin/Source/SequencePlugin/Public 

=========================================================================================================================

!!!BEFORE YOU CAN SAFELY USE THIS!!!
You must provide an encryption key implementation at SequenceEncryptor.cpp Function GetStoredKey,
This function must be implemented to provide a securely stored private key that will be used to encrypt &
decrypt client information. Failure to do so will result in NO information being stored or in the event you
do not use a securely stored key, can result in client information being stored insecurely on their systems.
!!!BEFORE YOU CAN SAFELY USE THIS!!!

=========================================================================================================================

The subset of the SequenceAPI can be found at

SequenceAPI.h

You can directly interact with the chain via Provider.h found in the ETH folder