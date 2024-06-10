This has some problems. Rarely, the server willreturn to the threads, when calling the assign_task method 0 to every value of the worker_args_t structure.
 The locks in  general seem to work, so I guess this is the fault of some of the generated functions, by rpcgen
