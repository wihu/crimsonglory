/// \file
/// \brief This class uses RakNet to detect missing files on one computer and to send them from another computer.
///
/// \ingroup RAKNET_AUTOPATCHER
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

#ifndef __AUTO_PATCHER_H
#define __AUTO_PATCHER_H

class RakPeerInterface;
class RakServerInterface;
class RakClientInterface;

struct Packet;

struct DownloadableFileDescriptor;
#include "NetworkTypes.h"
#include "DS_List.h"
#include "SHA1.h" 

/// \ingroup RAKNET_AUTOPATCHER
/// Results of autopatcher operations
enum SetFileDownloadableResult
{
	SET_FILE_DOWNLOADABLE_FAILED, /// Failed to allow a file to be downloaded, probably because it doesn't exist
	SET_FILE_DOWNLOADABLE_FILE_NO_SIGNATURE_FILE, /// Failed to create a signature file for some reason (you probably won't hit this)
	SET_FILE_DOWNLOADABLE_FILE_SIGNATURE_CHECK_FAILED, /// Downloaded a file, but the signature (see it as a checksum) doesn't match, so the download was corrupted.
	SET_FILE_DOWNLOADABLE_COMPRESSION_FAILED, /// zLib failed to compress a file for some reason
	SET_FILE_DOWNLOADABLE_SUCCESS, /// Success
};

/// \ingroup RAKNET_AUTOPATCHER 
/// \brief Autopatcher Subsystem
/// 
/// The autopatcher will update client files to match a given server fileset.  Files are transferred compressed.
/// This is a system on top of RakNet that is here for the convenience of users.
class AutoPatcher
{

public:
	/// Default Constructor
	AutoPatcher();
	
	/// Destructor
	~AutoPatcher();
	
	/// Frees all downloadable files.
	/// Frees the download directory prefix.
	void Clear( void );
	
	/// Set the ordering stream to send data on.  Defaults to 0.  Set this if you use ordered data for your game and don't want to hold up game data because of autopatcher file data
	/// \param[in] streamIndex The stream to use for autopatcher file data.  Same thing as the ordering stream for RakPeer::Send
	void SetOrderingStream( int streamIndex );
	
	/// Set which instance of RakPeerInterface to use for sends. Mutually exclusive with the other 2 overloads.
	/// \param[in] localSystem The instance of RakPeerInterface
	void SetNetworkingSystem( RakPeerInterface *localSystem );
	
	/// Set which instance of RakClientInterface to use for sends. Mutually exclusive with the other 2 overloads.
	/// \param[in] localSystem The instance of RakClientInterface
	void SetNetworkingSystem( RakClientInterface *localSystem );
	
	/// Set which instance of RakServerInterface to use for sends. Mutually exclusive with the other 2 overloads.
	/// \param[in] localSystem The instance of RakServerInterface
	void SetNetworkingSystem( RakServerInterface *localSystem );
	
	/// Set the value at which files larger than this will be compressed.
	/// Files smaller than this will not be compressed. Defaults to
	/// 1024. Only changes files later passed to SetFileDownloadable,
	/// not files already processed
	/// \param[in] boundary the new boundary for compression activation. 
	void SetCompressionBoundary( unsigned boundary );
	
	/// Creates a .sha file signature for a particular file.
	/// This is used by SetFileDownloadable with checkFileSignature as true
	/// 
	/// \param[in] filename the name of the file 
	/// \return true on success, false on file does not exist.
	static bool CreateFileSignature( char *filename );
	
	/// Makes a file downloadable Returns checkFileSignature - if true
	/// then check the associated .sha1 to make sure it describes our
	/// file.  checkFileSignature is useful to make sure a file wasn't
	/// externally modified by a hacker or a virus 
	/// 
	/// \param[in] filename the name of the file 
	/// \param[in] checkFileSignature if true then check the associated .sha1 to make sure it describes our file.  checkFileSignature is useful to make sure a file wasn't externally modified by a hacker or a virus.  Useful for servers that run a long time.
	/// \return See SetFileDownloadableResult
	SetFileDownloadableResult SetFileDownloadable( char *filename, bool checkFileSignature );
	
	/// Removes access to a file previously set as downloadable.
	/// \param[in] filename The name of the file 
	/// \return true on success, false on failure. 
	bool UnsetFileDownloadable( char *filename );
	
	/// Returns how many files are still in the download list.
	/// \pre Requires a previous call to OnAutopatcherFileList
	/// If returns >=1 filename and fileLength will be filled in to match
	/// the current file being downloaded
	/// A value of 0 for compressedFileLength indicates unknown
	/// \param[in] filename the file currently downloaded 
	/// \param[in] fileLength the size of the file currently downloaded 
	/// \param[in] fileDataIsCompressed true if the file is compressed 
	/// \param[in] compressedFileLength the size of the compressed file 
	/// \return The number of files still in the download list. 
	unsigned int GetDownloadStatus( char *filename, unsigned *fileLength, bool *fileDataIsCompressed, unsigned *compressedFileLength );
	
	///  Sets a base directory to look for and put all downloaded files in.
	///  For example, "Downloads"
	///  \param prefix the path to the source or the destination directory. 
	void SetDownloadedFileDirectoryPrefix( char *prefix );
	
	 /// Requests that the remote system send the directory of files that
	 /// are downloadable.  The remote system should get
	 /// ID_AUTOPATCHER_REQUEST_FILE_LIST.  When it does, it should call
	 /// SendDownloadableFileList with the playerID of the sender.  For
	 /// the client, you can put UNASSIGNED_PLAYER_ID for remoteSystem
	 /// \param[in] remoteSystem The remoteSystem to ask for its downloadable
	 /// file list.
	void RequestDownloadableFileList( const PlayerID remoteSystem );
	
	 /// If the packet identifier is ID_AUTOPATCHER_REQUEST_FILE_LIST, call this function with
	 /// packet->playerID
	 /// Sends a list of all downloadable files to the remote system
	 /// The remote system should get a packet with ID ID_AUTOPATCHER_FILE_LIST.
	 /// When it does, it should call
	 /// OnAutopatcherFileList with the packet from the network
	 /// \param[in] remoteSystem the player to send the list to. 
	void SendDownloadableFileList( const PlayerID remoteSystem );
	
	 /// If the packet identifier is ID_AUTOPATCHER_FILE_LIST, call this
	 /// function with the packet.  It will parse out all the files on the
	 /// remote system and request to download the ones we don't have or
	 /// don't match.  The remote system should get a packet with ID
	 /// ID_AUTOPATCHER_REQUEST_FILES for each file requested
	 /// We can specify to only accept files if we previously requested
	 /// them by a call to RequestDownloadableFileList.  Set
	 /// onlyAcceptFilesIfRequested to true to do this
	 /// After this function is called you can call GetDownloadStatus(...)
	 /// To find out which, if any, files are currently downloading
	 /// 
	 /// \param[in] packet the packet with id ID_AUTOPATCHER_FILE_LIST 
	 /// \param[in] onlyAcceptFilesIfRequested Does what the name says.  This means the server won't push files to you - you have to ask for them first.
	void OnAutopatcherFileList( Packet *packet, bool onlyAcceptFilesIfRequested );
	
	 /// If the packet identifier is ID_AUTOPATCHER_REQUEST_FILES, call
	 /// this function with the packet.  Reads the files from disk and
	 /// sends them to the specified system
	 /// \param[in] packet the packet with id ID_AUTOPATCHER_REQUEST_FILES
	void OnAutopatcherRequestFiles( Packet *packet );
	
	 /// If the packet identifier is ID_AUTOPATCHER_SET_DOWNLOAD_LIST,
	 /// call this function with the packet. Finalizes the list of files
	 /// that will be downloaded.
	 /// \param[in] packet the packet with id ID_AUTOPATCHER_SET_DOWNLOAD_LIST
	void OnAutopatcherSetDownloadList( Packet *packet );
	
	 /// If the packet identifier is ID_AUTOPATCHER_WRITE_FILE, call this
	 /// function with the packet.  Writes a file to disk.  There is
	 /// security to prevent writing files we didn't ask for
	 /// \param[in] packet the packet with id ID_AUTOPATCHER_WRITE_FILE
	 /// \return true on success
	bool OnAutopatcherWriteFile( Packet *packet );
	
	 /// \internal 
	 /// 
	 /// \return true on success 
	static bool GenerateSHA1( char *filename, char SHA1Code[ SHA1_LENGTH ] );
	
	 /// \internal 
	 /// \return true on success 
	static int GetFileLength( char *filename );
	
	 /// \internal 
	 /// \return true on success 
	static bool WriteFileWithDirectories( const char *path, char *data, unsigned dataLength );
	
protected:
	/// One of these 3 must be set in order to send data
	RakPeerInterface *rakPeerInterface;
	/// One of these 3 must be set in order to send data
	RakClientInterface *rakClientInterface;
	/// One of these 3 must be set in order to send data
	RakServerInterface *rakServerInterface;
	/// The directory to download into basically
	char *downloadPrefix;
	/// Files of length >= this will be compressed
	unsigned compressionBoundary;
	/// true if the downloadableFiles have been requested
	bool downloadableFilesRequested;
	/// The ordering stream to use
	int orderingStream;
	/// Directory of files that are downloadable
	DataStructures::List<DownloadableFileDescriptor*> downloadableFiles, downloadingFiles;
};

#endif
