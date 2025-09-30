#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  initscr();
  noecho();
  curs_set(FALSE);

  keypad(stdscr, TRUE); // enable arrow key

  char *options[] = {"Start", "Battery info", "Quit"};
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
      if( choice == 2) break; //Battery info
      else if(choice == 1) {	
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
        mvprintw(n_options + 1, 0, "Battery: %s%% (%s))", battery, status);
	mvprintw(n_options + 2, 0, "\nPress any key to continue...");
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
