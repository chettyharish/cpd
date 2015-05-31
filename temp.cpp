#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

struct macro {
	string macname;
	string macvalue;
};

struct target {
	string tarname;
	string deps[10];
	string tarcmd[10];
	string ntrdep[10];
};

int macrcnt;

string dolrep(string cmd, macro *mac) {
	size_t i;
	size_t p;
	char rem[] = "()";
	int j;

	for (i = 0; i < strlen(rem); i++) {
		cmd.erase(remove(cmd.begin(), cmd.end(), rem[i]), cmd.end());
	}

	for (j = 0; j < macrcnt; j++) {
		p = cmd.find('$' + mac[j].macname);
		if (p != string::npos) {
			cmd.replace(p, mac[j].macname.length() + 1, mac[j].macvalue);
		}
	}
	return cmd;
}

string dolrep(string cmd, string src, int i) {
	size_t p;
	string cf = ".c";
	if (i == 2) {
		p = cmd.find("$<");
	} else if (i == 1) {
		p = cmd.find("$@");
	}

	if (p != string::npos) {
		cmd.replace(p, src.length(), src);
	}
	p = cmd.find(".o");
	if (p != string::npos) {
		cmd.replace(p, cf.length(), cf);
	}
	return cmd;
}

class stack {
	target targ[20];
	int tos;
public:
	stack();
	void push(target);
	void pop();
	int isempty();
	int isfull();
	target top();
};

stack::stack() {
	tos = 0;
}

int stack::isempty() {
	return (tos == 0 ? 1 : 0);
}

int stack::isfull() {
	return (tos == 20 ? 1 : 0);
}

void stack::push(target t) {

	if (!isfull()) {
		targ[tos] = t;
		tos++;
	} else {
		cerr << "Stack overflow error!";
	}
}

void stack::pop() {
	if (!isempty()) {
		--tos;
	} else {
		cerr << "Stack is empty!";
	}
}

target stack::top() {
	return (targ[tos - 1]);
}

size_t nonempdcnt(target t) {
	size_t dcnt = 0;

	for (size_t j = 0; j < sizeof(t.deps) / sizeof(*t.deps); j++)
		if (t.deps[j] != "")
			dcnt++;
	return dcnt;
}

size_t nonempccnt(target t) {
	size_t ccnt = 0;

	for (size_t k = 0; k < sizeof(t.tarcmd) / sizeof(*t.tarcmd); k++)
		if (t.tarcmd[k] != "")
			ccnt++;
	return ccnt;
}

size_t nonempntrcnt(target t) {
	size_t ncnt = 0;

	for (size_t l = 0; l < sizeof(t.ntrdep) / sizeof(*t.ntrdep); l++)
		if (t.ntrdep[l] != "")
			ncnt++;
	return ncnt;

}

stack s;
int targcnt;

void buildtarstack(string tnam, target *tag) {
	int i;
	size_t j;
	for (i = 0; i < targcnt; i++) {
		if (tnam == tag[i].tarname) {
			if (nonempdcnt(tag[i]) == 0) {
				s.push(tag[i]);

				return;
			} else {
				s.push(tag[i]);

				for (j = 0; j < nonempdcnt(tag[i]); j++) {
					buildtarstack(tag[i].deps[j], tag);
				}

				return;
			}
		} else {
			if (i == (targcnt - 1))
				return;
		}
	}
}

int fexists(string f) {
	char fc[50];
	int f_d = 0;
	strcpy(fc, f.c_str());
	f_d = open(fc, O_RDONLY);
	if (f_d == -1) {
		close(f_d);
		return 0;
	} else {
		close(f_d);
		return 1;
	}

}

int tdiff(string f1, string f2) {
	char fc1[50], fc2[50];
	int f_d1 = 0;
	int f_d2 = 0;
	struct stat st1;
	struct stat st2;
	struct tm t1;
	struct tm t2;
	time_t rt1, rt2;

	strcpy(fc1, f1.c_str());
	strcpy(fc2, f2.c_str());

	memset(&t1, 0, sizeof(struct tm));
	memset(&t2, 0, sizeof(struct tm));

	f_d1 = open(fc1, O_RDONLY);
	f_d2 = open(fc2, O_RDONLY);

	if (f_d1 == -1) {
		return -1;
	} else {
		stat(fc1, &st1);
		strptime(ctime(&st1.st_mtime), "%a %b %d %H:%M:%S %Y", &t1);
		rt1 = mktime(&t1);
	}

	if (f_d2 == -1) {
		return -1;
	} else {
		stat(fc2, &st2);
		strptime(ctime(&st2.st_mtime), "%a %b %d %H:%M:%S %Y", &t2);
		rt2 = mktime(&t2);
	}

	if (rt1 < rt2) {
		close(f_d1);
		close(f_d2);
		return 1;
	} else if (rt1 > rt2) {
		close(f_d1);
		close(f_d2);
		return 2;
	} else {
		close(f_d1);
		close(f_d2);
		return 0;
	}
}

size_t charcnt(string str, const char ch) {

	size_t ct = 0;
	char n;
	int i = 0;
	char strchr[50];
	strcpy(strchr, str.c_str());
	while ((n = strchr[i++]), ((n == ch) ? ++ct : 0), n)
		;

	return ct;
}

int spawn(string command) {
	int status;
	char cmd[50];
	strcpy(cmd, command.c_str());
	char* arg_list[50];
	int argcnt = 0;
	char *tok;
	tok = strtok(cmd, " ");

	while (tok != NULL) {
		arg_list[argcnt] = tok;
		tok = strtok(NULL, " ");
		argcnt++;
	}
	arg_list[argcnt] = NULL;

	pid_t child_pid;
	child_pid = fork();
	if (child_pid != 0) {
		waitpid(child_pid, &status, 0);
		return child_pid;
	} else {
		char * argcmd[2];
		char * argcmd2[3];
		int f;
		string file;

		cout << "Executing command: " << command << endl;
		if ((charcnt(command, '>') > 0) || (charcnt(command, '<') > 0)) {
			if (charcnt(command, '>') > 0) {
				strcpy(argcmd[0], arg_list[0]);
				argcmd[1] = NULL;
				close(STDOUT_FILENO);
				f = open(arg_list[2], O_RDWR | O_CREAT | O_TRUNC, 0777);
				execvp(argcmd[0], argcmd);
			} else {
				argcmd2[0] = (char*) malloc(sizeof(char) * 128);
				argcmd2[1] = (char*) malloc(sizeof(char) * 128);
				argcmd2[2] = (char*) malloc(sizeof(char) * 128);

				strcpy(argcmd2[0], arg_list[0]);
				strcpy(argcmd2[1], arg_list[2]);
				argcmd2[2] = NULL;
				execvp(argcmd2[0], argcmd2);
			}
		} else {
			execvp(arg_list[0], arg_list);
		}

		fprintf(stderr, "an error occurred in execvp\n");
		abort();
	}
}

int runcmd(string command) {
	char *tok;
	char cmd[50];
	strcpy(cmd, command.c_str());
	char *comlist[50];
	tok = strtok(strdup(cmd), ";");
	int comcnt = 0;
	int rcnt = 0;
	int rc = 0;
	size_t tokcnt = 0;
	tokcnt = charcnt(command, ';');

	if (tokcnt > 0) {
		while (tok != NULL) {
			comlist[comcnt] = tok;
			tok = strtok(NULL, ";");
			comcnt++;
		}
		comlist[comcnt] = NULL;
		for (rcnt = 0; rcnt < comcnt; rcnt++) {
			rc = spawn(comlist[rcnt]);
		}
	} else {
		rc = spawn(command);
	}

	return rc;
}

int cntxdep(target ct) {
	size_t decnt;
	size_t check = 0;
	for (decnt = 0; decnt < nonempdcnt(ct); decnt++) {
		if (fexists(ct.deps[decnt]) == 1) {
			check++;
		}
	}

	if (check == nonempdcnt(ct)) {
		return 1;
	} else {
		return 0;
	}
}

int main(int argc, char * argv[]) {
	FILE * fp;
	char ch;
	int rdcomfg = 0;
	int nummac = 0;
	int numtar = 0;
	char buff[100][500];
	int buffro = 0;
	int buffco = 0;
	int curlin = 0;
	int curtab = 0;
	string tname;
	string fname;
	char filename[20];

	if (argc == 2) {
		if (fexists("mymake1")) {
			fname = "mymake1";
			strcpy(filename, fname.c_str());
			tname = argv[1];
		} else if (fexists("mymake2")) {
			fname = "mymake2";
			strcpy(filename, fname.c_str());
			tname = argv[1];
		} else if (fexists("mymake3")) {
			fname = "mymake3";
			strcpy(filename, fname.c_str());
			tname = argv[1];
		} else {
			cout << "Please enter a makefile name followed by -f." << endl;
			return 0;
		}
	} else if (argc == 4) {
		strcpy(filename, argv[2]);
		tname = argv[3];
	} else {
		cout << "Please enter details correctly" << endl;
		return 0;
	}

	fp = fopen(filename, "r");
	while ((ch = fgetc(fp)) != EOF) {
		if (ch == '=') {
			nummac++;
		}
		if (ch == ':') {
			numtar++;
		}
	}
	fclose(fp);

	fp = fopen(filename, "r");
	while ((ch = fgetc(fp)) != EOF) {
		if (ch == '#') {
			rdcomfg = 1;
		}
		if ((rdcomfg == 1) && (ch != '\n')) {
			continue;
		} else if ((rdcomfg == 1) && (ch == '\n')) {
			rdcomfg = 0;
			continue;
		}

		if (buffro < nummac) {
			if (ch == '\n') {
				buffro++;
				buffco = 0;
				continue;
			}
			buff[buffro][buffco] = ch;
			buffco++;
		} else if ((buffro - nummac) < numtar) {
			if (ch == '\n') {
				curtab = 0;
				curlin = 1;
				continue;
			} else if ((ch == '\t') && (curlin == 1)) {
				curtab = 1;
				buff[buffro][buffco] = '!';
				buffco++;
				continue;
			} else if ((ch != '\t') && (curlin == 1) && (curtab == 0)) {
				curlin = 0;
				buffro++;
				buffco = 0;
				buff[buffro][buffco] = ch;
				buffco++;
				continue;
			} else if ((ch != '\t') && (curtab == 1) && (curlin == 1)) {
				buff[buffro][buffco] = ch;
				buffco++;
				continue;
			}

			buff[buffro][buffco] = ch;
			buffco++;
		}

	}

	buff[buffro][buffco] = '\0';
	fclose(fp);

	char *t;
	char *t1;
	char *t2;
	macro mac[10];
	target tar[10];
	int tarcnt = 0;
	int depcnt = 0;
	int cmdcnt = 0;
	printf("\n");

	for (buffro = 0; buffro < nummac; buffro++) {
		t = strtok(buff[buffro], "=");
		mac[buffro].macname = t;
		t = strtok(NULL, "=");
		mac[buffro].macvalue = t;
	}
	macrcnt = buffro;

	for (buffro = nummac; buffro < (nummac + numtar); buffro++, tarcnt++) {
		t = strtok(buff[buffro], ":");
		tar[tarcnt].tarname = t;
		t = strtok(NULL, ":");

		if ((t[0] != '!') && (t != NULL)) {
			t1 = strtok(strdup(t), "!");
			t2 = strtok(strdup(t1), " ");

			while (t2 != NULL) {
				tar[tarcnt].deps[depcnt] = t2;
				t2 = strtok(NULL, " ");
				depcnt++;
			}

			t1 = strtok(strdup(t), "!");

			t1 = strtok(NULL, "!");
			while (t1 != NULL) {
				tar[tarcnt].tarcmd[cmdcnt] = dolrep(t1, mac);
				t1 = strtok(NULL, "!");
				cmdcnt++;
			}
		} else {
			size_t n;
			string str;
			t1 = strtok(strdup(t), ":");
			str = t1;
			n = count(str.begin(), str.end(), '!');

			if (n > 1) {
				t2 = strtok(strdup(t1), "!");
				while (t2 != NULL) {
					tar[tarcnt].tarcmd[cmdcnt] = dolrep(t2, mac);
					t2 = strtok(NULL, "!");
					cmdcnt++;
				}
			} else {
				string str2 = t1;
				tar[tarcnt].tarcmd[cmdcnt] = dolrep(str2.substr(1), mac);
				cmdcnt++;
			}
		}
		depcnt = 0;
		cmdcnt = 0;
	}

	targcnt = tarcnt;

	int tcnt = 0;
	size_t dcnt = 0;
	tarcnt = 0;
	int trvflg = 0;
	int ntrcnt = 0;

	for (tcnt = 0; tcnt < targcnt; tcnt++) {
		for (dcnt = 0; dcnt < nonempdcnt(tar[tcnt]); dcnt++) {
			for (tarcnt = 0; tarcnt < targcnt; tarcnt++) {
				if (tar[tcnt].deps[dcnt] == tar[tarcnt].tarname) {
					trvflg++;
				}
			}
			if (trvflg == 0) {
				tar[tcnt].ntrdep[ntrcnt] = tar[tcnt].deps[dcnt];
				ntrcnt++;
			}
			trvflg = 0;
		}
		ntrcnt = 0;
	}

	buildtarstack(tname, tar);

	target ct;
	size_t cdcnt;
	size_t decnt;
	int targetcnt;
	int numexi = 0;
	while (!s.isempty()) {
		ct = s.top();
		if (fexists(ct.tarname) == 1) {
			numexi = cntxdep(ct);
			for (decnt = 0; decnt < nonempdcnt(ct); decnt++) {
				if (numexi == 1) {
					if (tdiff(ct.tarname, ct.deps[decnt]) == 1) {
						for (cdcnt = 0; cdcnt < nonempccnt(ct); cdcnt++) {
							runcmd(ct.tarcmd[cdcnt]);
							break;
						}
					}
				} else {
					if (fexists(ct.deps[decnt]) == 0) {
						for (targetcnt = 0; targetcnt < targcnt; targetcnt++) {
							if (charcnt(tar[targetcnt].tarname, '.') == 2) {
								for (cdcnt = 0;
										cdcnt < nonempccnt(tar[targetcnt]);
										cdcnt++) {
									runcmd(
											dolrep(tar[targetcnt].tarcmd[cdcnt],
													ct.deps[decnt], 2));
								}
							} else if ((charcnt(tar[targetcnt].tarname, '.')
									== 1)
									&& (tar[targetcnt].tarname.length() == 2)) {
								for (cdcnt = 0;
										cdcnt < nonempccnt(tar[targetcnt]);
										cdcnt++) {
									runcmd(
											dolrep(tar[targetcnt].tarcmd[cdcnt],
													ct.deps[decnt], 1));
								}
							}

						}
					}
				}
				numexi = cntxdep(ct);
				if (numexi == 1) {
					if (tdiff(ct.tarname, ct.deps[decnt]) == 1) {
						for (cdcnt = 0; cdcnt < nonempccnt(ct); cdcnt++) {
							runcmd(ct.tarcmd[cdcnt]);
							break;
						}
					}
				}

			}
		} else {
			for (decnt = 0; decnt < nonempdcnt(ct); decnt++) {
				if (fexists(ct.deps[decnt]) == 1) {
					if (tdiff(ct.tarname, ct.deps[decnt]) == 1) {
						for (cdcnt = 0; cdcnt < nonempccnt(ct); cdcnt++) {
							runcmd(ct.tarcmd[cdcnt]);
						}
					}
				} else {

					for (targetcnt = 0; targetcnt < targcnt; targetcnt++) {
						if (charcnt(tar[targetcnt].tarname, '.') == 2) {
							for (cdcnt = 0; cdcnt < nonempccnt(tar[targetcnt]);
									cdcnt++) {
								runcmd(
										dolrep(tar[targetcnt].tarcmd[cdcnt],
												ct.deps[decnt], 2));
							}
						} else if ((charcnt(tar[targetcnt].tarname, '.') == 1)
								&& (tar[targetcnt].tarname.length() == 2)) {
							for (cdcnt = 0; cdcnt < nonempccnt(tar[targetcnt]);
									cdcnt++) {
								runcmd(
										dolrep(tar[targetcnt].tarcmd[cdcnt],
												ct.deps[decnt], 1));
							}
						}
					}
				}
			}

			for (cdcnt = 0; cdcnt < nonempccnt(ct); cdcnt++) {
				runcmd(ct.tarcmd[cdcnt]);
			}
		}
		s.pop();
	}

	exit(0);
}
