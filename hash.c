#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "redirect.h"
#include "pipes.h"
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include "jobs.h"
#include "builtins.h"
#include <stdbool.h>

int status, pid_ch1, pid_ch2, pid;
int pipefd[2];
int rdrct_i, rdrct_t, pipe_i;
char *pipel_args[2000];
char *piper_args[2000];
char *args[2000];
char *inpt_tkns[2000];
/*Job Control Variables*/
int job_status[2000];		// 0 means stopped and 1 means running 2 means done 3 means dead
char *job_commands[2000];
int job_cnt = 0;
int job_pgroup[2000];
char *doneJobs;
int current_job = -1;
int rdrctIndices[20];
int rdrctCnt;


static void sig_int(int signo)
{

    printf("Sending SIGINT to group:%d\n", pid_ch1);
    kill(-pid_ch1, SIGINT);

}

static void sig_tstp(int signo)
{

    printf("Sending SIGTSTP to group:%d\n", pid_ch1);
    kill(-pid_ch1, SIGTSTP);
}

static void handle_sigchld(int signo)
{

    int saved_errno = errno;
    int sgc_pid;		//pid of the child of SIGCHLD'd
    while ((sgc_pid = waitpid(-1, 0, WNOHANG)) > 0) {

	/*Delete the backgroundeprocess from the Job Table */
	int pgid = sgc_pid;
	int job_pg_index = -1;

	for (int i = 0; i < job_cnt; i++) {
	    if (job_pgroup[i] == pgid) {
		job_pg_index = i;
	    }
	}

	if (job_pg_index == -1) {

	    continue;

	} else {

	    job_status[job_pg_index] = 2;
	    current_job = get_active_job(job_cnt, job_status);

	}

    }

    errno = saved_errno;

}

static void yash_sig_int(int signo){}

static void yash_sig_tstp(int signo){}



int main(int argc, const char *argv[])
{

    // Registers process SIGCHLD
    struct sigaction sa;
    char *unmod_inpt_str, *crnt_wd, *crnt_usr;
    sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sa, 0) == -1) {
	    perror(0);
	    exit(1);
    }

    while (1) {

        signal(SIGINT, yash_sig_int);
        signal(SIGTSTP, yash_sig_tstp);

        int tkn_indx = 0;
        char *file_name;
        int i[20];
        char *inpt_str = malloc(1024);
        char *tkn_ptr;
        int tkn_num = 0;
        
        crnt_wd = getcwd(NULL, 1024);
        crnt_usr = getenv("LOGNAME");
        
        printf("%s (%s) > ", crnt_usr,  crnt_wd);

        if (fgets(inpt_str, 2000, stdin) == NULL) {
            exit(0);
        }
        unmod_inpt_str = strdup(inpt_str);

        tkn_ptr = strtok(inpt_str, " \n");
        if (tkn_ptr == NULL) {
            continue;
        }

        while (tkn_ptr != NULL) {
            inpt_tkns[tkn_num] = tkn_ptr;
            tkn_ptr = strtok(NULL, " \n");
            tkn_num += 1;
        }
        inpt_tkns[tkn_num] = NULL;

        /*Build args and vars for redirection operation */
        rdrct_i = 0;
        rdrctCnt = 0;
        pipe_i = find_pipes(inpt_tkns, tkn_num, tkn_indx);

        while ((rdrct_i = get_rdrct(inpt_tkns, tkn_num, rdrct_i)) != -1) {

            rdrctIndices[rdrctCnt] = rdrct_i;
            rdrctCnt += 1;
            rdrct_i += 1;

        }

        if (pipe_i != -1) {
            
            if (pipe(pipefd) == -1) {
                perror("pipe");
                exit(-1);
            }
            build_pipe_args(pipel_args, piper_args, 0, pipe_i, tkn_num, inpt_tkns);
        }

        int j, num_builtins, builtin_flag;
        num_builtins = hash_num_builtins();
        builtin_flag = false; 
        for (j = 0; j < num_builtins; j++) {
            
            if (strcmp(inpt_tkns[0], builtin_str[j]) == 0) {
                (*builtin_func[j]) (inpt_tkns);
                builtin_flag = true; 
            }

        } 
        if ( builtin_flag) {
            continue;
        }

        if (strcmp(inpt_tkns[0], "jobs") == 0) {
            
            printJobs(job_cnt, job_status, current_job, job_commands);
            continue;

        } else if (strcmp(inpt_tkns[0], "fg") == 0) {

            job_status[current_job] = 1;
            kill(job_pgroup[current_job], SIGCONT);
            if (signal(SIGINT, sig_int) == SIG_ERR)
            printf("signal(SIGINT) error");

            if (signal(SIGTSTP, sig_tstp) == SIG_ERR)
            printf("signal(SIGTSTP) error");

            pid = waitpid(-1, &status, WUNTRACED);

            if (WIFEXITED(status)) {
                job_cnt -= 1;
                printf("child %d exited, status=%d\n", pid,
                       WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("child %d killed by signal %d\n", pid,
                       WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("%d stopped by signal %d\n", pid, WSTOPSIG(status));
                job_status[(job_cnt - 1)] = 0;
            }
            continue;

        } else if (strcmp(inpt_tkns[0], "bg") == 0) {

            job_status[current_job] = 1;
            kill(job_pgroup[(job_cnt - 1)], SIGCONT);
            continue;

        } else {

            pid_ch1 = fork();
            if (pid_ch1 > 0) {
                /*Parent process */
                // printf("Child 1 pid = %d\n",pid_ch1);
                if (pipe_i != -1) {
                    
                    pid_ch2 = fork();
                    if (pid_ch2 > 0) {

                        // printf("Child 2 pid = %d\n", pid_ch2);
                        if (signal(SIGINT, sig_int) == SIG_ERR)
                            printf("signal(SIGINT) error");

                        if (signal(SIGTSTP, sig_tstp) == SIG_ERR)
                            printf("signal(SIGTSTP) error");

                        close(pipefd[0]);	//close the pipe in the parent
                        close(pipefd[1]);
                        int count = 0;
                        int stpd_cnt = 0;
                        while (count < 2) {
                            
                            pid = waitpid(-1, &status, WUNTRACED);
                            if (pid == -1) {
                                perror("waitpid");
                                break;
                            }
                            if (WIFEXITED(status)) {
                                printf("child %d exited, status=%d\n", pid, WEXITSTATUS(status));
                                count++;
                            } else if (WIFSIGNALED(status)) {
                                printf("child %d killed by signal %d\n", pid, WTERMSIG(status));
                                count++;
                            } else if (WIFSTOPPED(status)) {
                                
                                printf("%d stopped by signal %d\n", pid, WSTOPSIG(status));
                                stpd_cnt += 1;
                                
                                if (stpd_cnt == 2) {
                                    // Stop both process before building jobs table
                                    job_status[job_cnt] = 0;
                                    job_commands[job_cnt] = unmod_inpt_str;
                                    job_pgroup[job_cnt] = pid_ch1;
                                    current_job = job_cnt;
                                    job_cnt += 1;
                                    break;
                                }

                            }

                        }

                    } else if (pid_ch2 == 0) {
                        //Child Process 2
                        sleep(1);
                        setpgid(0, pid_ch1);
                        close(pipefd[1]);	// Closing write end of pipe
                        dup2(pipefd[0], STDIN_FILENO);
                        char *rdr_args[2000];
                        int frst_rdrct = -1;

                        if (rdrctCnt > 0) {
                            int k;
                            int hndled_rdrcts = 0;
                            for (k = 0; k < rdrctCnt; k++) {
                            if (rdrctIndices[k] > pipe_i) {
                                if (frst_rdrct == -1) {
                                frst_rdrct = rdrctIndices[k];
                                }
                                file_name =
                                inpt_tkns[rdrctIndices[k] + 1];
                                rdrct_t =
                                rdrct_type(inpt_tkns,
                                       rdrctIndices[k]);
                                handle_rdrct(file_name, rdrct_t);
                            }

                            }
                            if (frst_rdrct != -1) {
                            build_rdrct_args(frst_rdrct, rdr_args,
                                     inpt_tkns, (pipe_i + 1));
                            execvp(rdr_args[0], rdr_args);
                            } else {
                            /*Means there's redirects but none after the pipe */
                            execvp(piper_args[0], piper_args);
                            }
                        } else {
                            execvp(piper_args[0], piper_args);
                        }
                    }
                } else if (strcmp(inpt_tkns[(tkn_num - 1)], "&") == 0) {
                    
                    /*Puts bg process in process table */
                    job_status[job_cnt] = 1;
                    job_commands[job_cnt] = strdup(unmod_inpt_str);
                    job_pgroup[job_cnt] = pid_ch1;
                    current_job = job_cnt;
                    job_cnt += 1;
                    continue;

                } else {
                    
                    if (signal(SIGINT, sig_int) == SIG_ERR)
                        printf("signal(SIGINT) error");

                    if (signal(SIGTSTP, sig_tstp) == SIG_ERR)
                        printf("signal(SIGTSTP) error");

                    waitpid(pid_ch1, &status, 0);

                    if (WIFEXITED(status)) {
                    //printf("child %d exited, status=%d\n", pid, WEXITSTATUS(status));
                    } else if (WIFSIGNALED(status)) {
                    //printf("child %d killed by signal %d\n", pid, WTERMSIG(status));
                    } else if (WIFSTOPPED(status)) {
                        //printf("%d stopped by signal!%d\n", pid,WSTOPSIG(status));
                        job_status[job_cnt] = 0;
                        job_commands[job_cnt] = unmod_inpt_str;
                        job_pgroup[job_cnt] = pid_ch1;
                        current_job = job_cnt;
                        job_cnt += 1;
                    }

                }
            } else {
                
                //Child 1 process
                setpgid(getpid(), getpid());
                if (pipe_i != -1) {
                    
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);

                    if ((rdrctCnt > 0) && rdrctIndices[0] < pipe_i) {

                        build_rdrct_args(rdrctIndices[0], args, inpt_tkns, 0);
                        int rdrct_j = 0;
                        while (rdrct_j < rdrctCnt && rdrctIndices[rdrct_j] < pipe_i) {
                            file_name =  inpt_tkns[(rdrctIndices[rdrct_j] + 1)];
                            rdrct_t =  rdrct_type(inpt_tkns, rdrctIndices[rdrct_t]);
                            handle_rdrct(file_name, rdrct_t);
                            rdrct_j += 1;
                        }
                        execvp(args[0], args);

                    } else {
                        execvp(pipel_args[0], pipel_args);
                    }

                } else if (rdrctCnt > 0) {
                    int j;
                    build_rdrct_args(rdrctIndices[0], args, inpt_tkns, 0);
                    for (j = 0; j < rdrctCnt; j++) {
                        file_name = inpt_tkns[(rdrctIndices[j] + 1)];
                        rdrct_t = rdrct_type(inpt_tkns, rdrctIndices[j]);
                        handle_rdrct(file_name, rdrct_t);
                    }
                    execvp(args[0], args);
                }

                if (strcmp(inpt_tkns[tkn_num - 1], "&") == 0) {
                    inpt_tkns[tkn_num - 1] = NULL;
                    execvp(inpt_tkns[0], inpt_tkns);
                }

                execvp(inpt_tkns[0], inpt_tkns);

            }
        }

        free(crnt_wd);

    }
    return 0;
}
