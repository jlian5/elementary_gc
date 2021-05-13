# Example output from /usr/bin/time -v sleep 1:
#     Command being timed: "sleep 1"
#     User time (seconds): 0.00
#     System time (seconds): 0.00
#     Percent of CPU this job got: 0%
#     Elapsed (wall clock) time (h:mm:ss or m:ss): 0:01.12
#     Average shared text size (kbytes): 0
#     Average unshared data size (kbytes): 0
#     Average stack size (kbytes): 0
#     Average total size (kbytes): 0
#     Maximum resident set size (kbytes): 1860
#     Average resident set size (kbytes): 0
#     Major (requiring I/O) page faults: 0
#     Minor (reclaiming a frame) page faults: 71
#     Voluntary context switches: 2
#     Involuntary context switches: 2
#     Swaps: 0
#     File system inputs: 0
#     File system outputs: 0
#     Socket messages sent: 0
#     Socket messages received: 0
#     Signals delivered: 0
#     Page size (bytes): 4096
#     Exit status: 0

library(tidyverse)

cols <- c("command", "user_time", "system_time", "cpu_percent", "elapsed_time", "avg_shared_txt_size", "avg_unshared_data_size", "avg_stack_size", "avg_total_size", "max_rss", "avg_rss", "maj_pf", "min_pf", "vol_context_switches", "invol_context_switches", "swps", "fs_in", "fs_out", "socket_msgs_sent", "socket_msgs_recv", "sigs_deliv", "pg_size", "exit_status")

gc_data <- read_csv("gc_stats.csv", col_names=cols)
gc_data$command <- str_extract(gc_data$command, "tester-[0-9]*")
gc_data$type <- "gc"
gc_data$cpu_percent <- as.numeric(str_extract(gc_data$cpu_percent, "[0-9]*")) / 100

malloc_data <- read_csv("malloc_stats.csv", col_names=cols)
malloc_data$command <- str_extract(malloc_data$command, "tester-[0-9]*")
malloc_data$type <- "glibc"
malloc_data$cpu_percent <- as.numeric(str_extract(malloc_data$cpu_percent, "[0-9]*")) / 100

print(sprintf("number of rows in gc_data: %d, number of rows in malloc_data: %d\n", nrow(gc_data), nrow(malloc_data)))

data <- rbind(gc_data, malloc_data)
data$command <- factor(data$command, level=paste0("tester-", 1:13))

tester_plot <- data %>% ggplot(aes(x=command, y=max_rss, col=type)) + geom_boxplot() + coord_flip()
ggsave(plot=tester_plot, filename="testers.png", width=6, height=4, dpi=400)

comparison_plot <- data %>% gather("key", "value", -command, -type) %>% ggplot(aes(value, fill=type)) + facet_wrap(~ key, scales="free") + geom_boxplot()
ggsave(plot=comparison_plot, filename="comparison.png", width=12, height=6, dpi=400)


