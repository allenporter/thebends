// song-info.c
// Author: Allen Porter <allen@thebends.org>
//
// Gets information for the current song using the MobileMusicPlayer library.

#include <CoreFoundation/CoreFoundation.h>
#include <MobileMusicPlayer/MobileMusicPlayer.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <err.h>

void DumpSongInfo(int conn, int ident) {
  printf("Artist: %s\n",
         PCCopyStringValueForPropertyForIdentifier(
           conn, kPropertyArtist, ident));
  printf("Song Title: %s\n",
         PCCopyStringValueForPropertyForIdentifier(
           conn, kPropertySongTitle, ident));
  printf("Album: %s\n",
         PCCopyStringValueForPropertyForIdentifier(
           conn, kPropertyAlbumTitle, ident));
}

int main() {
  int running = PCIsPlayerRunning();
  if (running != 1) {
    errx(1, "PCIsPlayerRunning(): MobileMusicPlayer is not running");
  }

  if (PCGetPlaybackState() != kPlayerPlaying) {
    errx(1, "PCGetPlaybackState(): Song is not currently playing");
  }

  int song_index = PCPlayerGetCurrentPlayingSong();
  if (song_index < 0) {
    errx(1, "PCPlayerGetCurrentPlayingSong(): Couldn't get current song");
  }

  int count = PCPlayerGetSongCount();
  printf("Playlist size = %d\n", count);
  int time = PCGetCurrentPlayingSongTime();
  printf("Current Time = %d (ms)\n", time);
  int duration = PCGetCurrentPlayingSongDuration();
  printf("Duration = %d (ms)\n", duration);

  int conn = PCCreateIAPConnection();
  if (conn == 0) {
    errx(1, "PCCreateIAPConnection() failed");
  }
  DumpSongInfo(conn, PCPlayerGetIdentifierAtIndex(NULL, song_index));
  PCCloseConnection(conn);
}
