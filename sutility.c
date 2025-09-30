#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>

typedef struct {
  double total_gb;
  double used_gb;
  double available_gb;
  double used_percentage;
} DiskUsage;

int get_disk_usage(const char* path, DiskUsage* usage, int* n_options) {
  struct statvfs stat;

  // statvfs() gets file system statistics. it returns 0 on success.
  if (statvfs(path, &stat) != 0) {
    // if statvfs fails, print the error and return -1
    mvprintw(*n_options+1, 0, "statvfs() failed");
    return -1;
  }

  // the sizes are calculated from the number of blocks and block size.
  // we use unsigned long long to prevent integer overflow on large disks.
  
  unsigned long long total_space = stat.f_blocks * stat.f_bsize;
  // f_bavail is the number of free blocks available to non-superusers.
  // this is typically the value you want to show a regular user.
  unsigned long long available_space = stat.f_bavail * stat.f_bsize;

  // f_bfree is the total number of free blocks.
  unsigned long long free_space = stat.f_bfree * stat.f_bsize;

  unsigned long long used_space = total_space - free_space;

  // convert byte values to Gigabytes (GB) for readability
  // 1 GB = 1024 * 1024 * 1024 bytes.
  usage->total_gb = (double)total_space / (1024*1024*1024);
  (*usage).available_gb = (double)available_space / (1024*1024*1024);
  (*usage).used_gb = (double)used_space / (1024*1024*1024);

  //Calculate the percentage of the disk that is used
  if (total_space > 0) {
    usage->used_percentage = ((double)used_space/total_space) * 100.0;
  } else {
    usage->used_percentage = 0.0;
  }

  return 0; // return 0 to indicate success
}

int main() {
  initscr();
  noecho();
  curs_set(FALSE);

  keypad(stdscr, TRUE); // enable arrow key

  char *options[] = {"Disk info", "Battery info", "Quit"};
  int choice = 0;
  int ch;

  int n_options = 3;

  while (1) {
    clear();
    for (int i =0; i < 3; i++) {
      if (i == choice) attron(A_REVERSE);
      mvprintw(i, 0, "%s", options[i]);
      if (i == choice) attroff(A_REVERSE);
    }
    refresh();
    ch = getch();

    if (ch == 'q') break;
    else if (ch == KEY_UP && choice > 0) choice--;
    else if (ch == KEY_DOWN && choice < n_options - 1) choice++;
    else if (ch == '\n' || ch == KEY_ENTER) {
      if( choice == 2) break; // quit
      else if (choice == 0) {
        const char* path_to_check = "/";

	      // create an instance of our struct to hold the results
	      DiskUsage disk_info;

        mvprintw(n_options + 1, 0, "Attempting to get disk usage for path: '%s'\n", path_to_check);
        
	      if (get_disk_usage(path_to_check, &disk_info, &n_options) == 0) {

          // If the function returns 0, it was successful.
          mvprintw(n_options + 1, 0, "--------------------------------------\n");
          mvprintw(n_options + 2, 0, "Disk Usage Information:\n");
          mvprintw(n_options + 3, 0, "Total Space:     %.2f GB\n", disk_info.total_gb);
          mvprintw(n_options + 4, 0, "Used Space:      %.2f GB (%.1f%%)\n", disk_info.used_gb, disk_info.used_percentage);
          mvprintw(n_options + 5, 0, "Available Space: %.2f GB\n", disk_info.available_gb);
          mvprintw(n_options + 6, 0, "--------------------------------------\n");

          mvprintw(n_options + 7, 0, "\nPress any key to continue...");
	        getch();
	      } else {
          // If the function returns -1, an error occurred.
          mvprintw(n_options + 1, 0, "Failed to retrieve disk usage information\n");
          mvprintw(n_options + 2, 0, "\nPress any key to continue...");
	        getch();
          // stderr
          return 1; // Exit with an error code
        }
      }
      else if(choice == 1) {	// battery info
        FILE *fp;
        char battery[20], status[20];

        // Read capacity
        fp = fopen("/sys/class/power_supply/BAT0/capacity", "r");
        if (fp != NULL) {
          fgets(battery, sizeof(battery), fp);
          fclose(fp);
        } else {
          // handle error
        }

        // Read charging status
        fp = fopen("/sys/class/power_supply/BAT0/status", "r");
        if (fp != NULL) {
          fgets(status, sizeof(status), fp);
          fclose(fp);
        } else {
          // handle error
        }
        battery[strcspn(battery, "\n")] = 0;
        status[strcspn(status, "\n")] = 0;
        
        mvprintw(n_options + 1, 0, "--------------------------------------\n");
	      mvprintw(n_options + 2, 0, "Battery info: %s%% (%s%%)", battery, status);
        mvprintw(n_options + 3, 0, "--------------------------------------");
	      mvprintw(n_options + 4, 0, "\nPress any key to continue...");
	      getch();
      }
      else {
        //handle Start or Settings
	mvprintw(n_options + 1, 0, "You selected: %s", options[choice]);
	getch();
      }
    }
  }

  endwin();
  return 0;
}
