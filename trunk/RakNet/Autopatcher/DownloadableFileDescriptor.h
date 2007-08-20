
/// 
/// \ingroup RAKNET_AUTOPATCHER
/// \brief \b [Internal] Encapsulates a file, and all data needed to transmit, download, and verify it for the autopatcher./// \file
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __DOWNLOADABLE_FILE_DESCRIPTOR
#define __DOWNLOADABLE_FILE_DESCRIPTOR

#include "BitStream.h"
#include "SHA1.h" 

/// \ingroup RAKNET_AUTOPATCHER
/// \brief File information.
///
/// A file descriptor contains informations used by the autopatcher to 
/// detect whether or not a file need to be updated or not. 

struct DownloadableFileDescriptor
{
	/// Default Constructor
	DownloadableFileDescriptor();
	
	/// Destructor
	~DownloadableFileDescriptor();
	
	/// Clear the file descriptor
	void Clear( void );
	
	/// Write the header to a bitstream
	/// @param[out] out The bitstream to write to
	void SerializeHeader( RakNet::BitStream *out );
	
	/// Write the SHA1 to a bitstream
	/// @param[out] out The bitstream to write to
	void SerializeSHA1( RakNet::BitStream *out );
	
	/// Write the file data to a bitstream
	/// @param[out] out The bitstream to write to
	void SerializeFileData( RakNet::BitStream *out );
	
	/// Read the header from a bitstream
	/// @param[in] in The bitstream to read from
	bool DeserializeHeader( RakNet::BitStream *in );
	
	/// Read the SHA1 file signature from a bitstream
	/// @param[in] in The bitstream to read from
	bool DeserializeSHA1( RakNet::BitStream *in );
	
	/// Read the file data from a bitstream
	/// @param[in] in The bitstream to read from
	bool DeserializeFileData( RakNet::BitStream *in );
	
	/// The filename obviously
	char *filename;
	
	/// The length of the file, in bytes
	unsigned fileLength;
	
	/// True if data is compressed using zlib 
	bool fileDataIsCompressed;
	
	///  The size of the compressed file 
	unsigned compressedFileLength;
	
	///  The SHA-1 hash key 
	char SHA1Code[ SHA1_LENGTH ];
	
	///  The data of the file. 
	char *fileData;
};

#endif
