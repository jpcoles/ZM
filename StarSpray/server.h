#ifndef SERVER_H

int svr_startSimulation(int fd);
int svr_stopSimulation(int fd);
int receiveICs(int fd);
int svr_sendNextFrame(int current_frame, int fd);
int write_config_file(char *filename);

#endif
