/// \file
/// \brief Creates instances of RakVoice
///
/// \ingroup RAKNET_RAKVOICE
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
/// Usage of Raknet is subject to the appropriate licence agreement.
/// "Shareware" Licensees with Rakkarsoft LLC are subject to the
/// shareware license found at
/// http://www.rakkarsoft.com/shareWareLicense.html
/// "Commercial" Licensees are subject to the commercial license found at
/// http://www.rakkarsoft.com/sourceCodeLicense.html
/// Custom license users are subject to the terms therein.
/// All other users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __RAK_VOICE_FACTORY_H
#define __RAK_VOICE_FACTORY_H

class RakVoiceInterface;

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else 
// Unix needs no export, but for name mangling, keep the function name
// clean. If you omit the 'extern "C"', the .so names will be
// compiler dependent.
#define EXPORT extern "C"
#endif


/// \ingroup RAKNET_RAKVOICE
/// \class RakVoiceFactory 
/// \brief Create an instance of RakVoice
///
/// \sa RakVoice.h
#if defined(_USRDLL)

class EXPORT RakVoiceFactory
#else

class RakVoiceFactory
#endif
{

public:
	/// Returns a new instance of the RakVoiceInterface.  Pass which port you want the socket to use
	/// \return a new instance of RakVoiceInterface 
	static RakVoiceInterface* GetRakVoiceInterface( void );
	
	///  Destroys an instance of RakVoice
	/// @param[in] i The interface to destroy 
	static void DestroyRakVoiceInterface( RakVoiceInterface* i );
};

#endif
