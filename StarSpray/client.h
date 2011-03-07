#ifndef CLIENT_H

int client_start(int argc, char **argv);
int connect_to_server();

void client_start_simulation();
void client_stop_simulation();
void client_pause_simulation();
void client_resume_simulation();
void client_toggle_pause();
void client_load_next_frame();
void client_clear_particles();

#endif
