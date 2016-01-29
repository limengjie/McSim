# By default, the unit of timing parameters is 'tick', not 'cycle'.

# if the application does not finish until it executes 'max_total_instrs',
# the simulation quits.
max_total_instrs            = 3000000000
# stack size per hardware thread
stack_sz                    = 2^23

# if true, none of the instructions executed on Pin is delivered to McSim.
pts.skip_all_instrs         = false
pts.simulate_only_data_caches = false
pts.show_l2_stat_per_interval = false

pts.num_hthreads            = 257
pts.num_hthreads_per_l1$    = 4
pts.num_l1$_per_l2$         = 8
pts.num_mcs                 = 8
# display simulation statistics when every pts.print_interval
# instruction is executed.
pts.print_interval          = 10^6
pts.is_race_free_application = true
pts.max_acc_queue_size       = 1000

pts.lsu.to_l1i_t_for_x87_op = 8
pts.lsu.to_l1i_t            = 8
pts.lsu.to_l1d_t            = 8
pts.lsu.branch_miss_penalty = 640             # unit: tick
pts.lsu.process_interval    = 80              # unit: tick
pts.lsu.bypass_tlb          = true
pts.lsu.consecutive_nack_threshold = 200000   # unit: instruction
# pts.lsu.num_bp_entries stands for the number of entries
# in a branch predictor.  currently it is assumed that each
# hardware thread has a branch predictor.
pts.lsu.num_bp_entries      = 256
# how many bits of global branch history information is XORed
# with branch instruction addresses.  Please check 
# 'Combining Branch Predictors' by McFarling, 1993 for 
# further information
pts.lsu.gp_size             = 60
pts.lsu.spinning_slowdown   = 5

pts.l1i$.num_sets           = 16
pts.l1i$.num_ways           = 4
# which part of the address is mapped into a set
pts.l1i$.set_lsb            = 6
pts.l1i$.process_interval   = 80
pts.l1i$.to_lsu_t           = 8               # unit: tick
pts.l1i$.to_l2_t            = 160
# for how many ticks a cache is used per access
pts.l1i$.acc_t              = 160
pts.l1i$.num_sets_per_subarray = 8
pts.l1i$.always_hit         = false

pts.l1d$.num_sets           = 32
pts.l1d$.num_ways           = 4
pts.l1d$.set_lsb            = 6
pts.l1d$.process_interval   = 80
pts.l1d$.to_lsu_t           = 8
pts.l1d$.to_l2_t            = 160
pts.l1d$.acc_t              = 160
pts.l1d$.num_sets_per_subarray = 8
pts.l1d$.always_hit         = false

pts.l2$.num_sets            = 256
pts.l2$.num_ways            = 16
pts.l2$.set_lsb             = 6
pts.l2$.process_interval    = 40
pts.l2$.to_l1_t             = 160
pts.l2$.to_dir_t            = 320
pts.l2$.to_xbar_t           = 320
pts.l2$.num_banks           = 4
# how many flits are needed for a packet with data.  it is 
# assumed that a packet without data need a single flit.
pts.l2$.num_flits_per_packet  = 3
pts.l2$.acc_t                 = 320
pts.l2$.num_sets_per_subarray = 16
pts.l2$.always_hit            = false

pts.dir.set_lsb              = 6
pts.dir.process_interval     = 80
pts.dir.to_mc_t              = 80
pts.dir.to_l2_t              = 160
pts.dir.to_xbar_t            = 160
pts.dir.cache_sz             = 8192
pts.dir.num_flits_per_packet = 3
pts.dir.num_sets             = 1024
pts.dir.num_ways             = 16
pts.dir.has_directory_cache  = false

# NoC type = ring/mesh/xbar
pts.noc_type                 = xbar
pts.xbar.to_dir_t            = 320
pts.xbar.to_l2_t             = 320
pts.xbar.process_interval    = 80

# please check 'Future Scaling of Processor-Memory
# Interfaces' by Jung Ho Ahn at el. SC09 for further
# details on the concept of VMDs.
pts.mc.process_interval      = 240
pts.mc.to_dir_t              = 2400
pts.mc.interleave_base_bit   = 12
pts.mc.interleave_xor_base_bit = 18
pts.mc.num_ranks_per_mc      = 1
pts.mc.num_vmds_per_rank     = 1
pts.mc.num_banks_per_vmd     = 16
# parameters that start with 'pts.mc.t[capital letter]'
# have the unit of 'pts.mc.process_interval' ticks.
pts.mc.tRCD         = 14
pts.mc.tRP          = 46
pts.mc.tRR          = 1
pts.mc.tCL          = 11
pts.mc.tBL          = 1
pts.mc.tWRBUB       = 0
pts.mc.tRWBUB       = 0
pts.mc.tRRBUB       = 0
pts.mc.tXP          = 0
pts.mc.tEP          = 0
pts.mc.tWTR         = 0
pts.mc.mini_rank    = false
pts.mc.req_window_sz = 32
pts.mc.rank_interleave_base_bit = 14
pts.mc.vmd_interleave_base_bit  = 14
pts.mc.bank_interleave_base_bit = 14
pts.mc.vmd_page_sz  = 4096
pts.mc.rbol_page_size    = 512
pts.mc.scheduling_policy = closed
pts.mc.use_rbol          = true
pts.mc.refresh_interval  = 800000
pts.mc.num_pages_per_bank = 8192
pts.mc.num_cached_pages_per_bank = 32
pts.mc.par_bs = true
pts.mc.full_duplex = true
pts.mc.read_first = true
pts.mc.rbol_bypass_interval  = 200000
pts.mc.policy_threshold      = 40
pts.mc.rbol_bypass_threshold = 40
pts.mc.rbol_bypass_tRP       = 18
pts.mc.is_fixed_latency = false
pts.mc.display_os_page_usage = false

pts.l1dtlb.num_entries  = 64
pts.l1dtlb.process_interval   = 80
pts.l1dtlb.to_lsu_t     = 8
pts.l1dtlb.page_sz_log2 = 22
pts.l1dtlb.miss_penalty = 800

pts.l1itlb.num_entries  = 32
pts.l1itlb.process_interval   = 80
pts.l1itlb.to_lsu_t     = 8
pts.l1itlb.page_sz_log2 = 12
pts.l1itlb.miss_penalty = 800

print_md = true

pts.use_trace_files = true

max_total_instrs            = 1000000000

trace_file_name = traces/433.4002.addr
trace_file_name = traces/433.4002.addr
trace_file_name = traces/433.88.addr
trace_file_name = traces/433.88.addr
trace_file_name = traces/433.9102.addr
trace_file_name = traces/433.1664.addr
trace_file_name = traces/433.2809.addr
trace_file_name = traces/433.2809.addr
trace_file_name = traces/450.1208.addr
trace_file_name = traces/450.1208.addr
trace_file_name = traces/450.2193.addr
trace_file_name = traces/450.2193.addr
trace_file_name = traces/450.3514.addr
trace_file_name = traces/450.3514.addr
trace_file_name = traces/450.1824.addr
trace_file_name = traces/450.1824.addr
trace_file_name = traces/459.4929.addr
trace_file_name = traces/459.27331.addr
trace_file_name = traces/459.20380.addr
trace_file_name = traces/459.28777.addr
trace_file_name = traces/459.28777.addr
trace_file_name = traces/459.13843.addr
trace_file_name = traces/459.13843.addr
trace_file_name = traces/459.13843.addr
trace_file_name = traces/470.1391.addr
trace_file_name = traces/470.6035.addr
trace_file_name = traces/470.6035.addr
trace_file_name = traces/470.13190.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/433.4002.addr
trace_file_name = traces/433.4002.addr
trace_file_name = traces/433.88.addr
trace_file_name = traces/433.88.addr
trace_file_name = traces/433.9102.addr
trace_file_name = traces/433.1664.addr
trace_file_name = traces/433.2809.addr
trace_file_name = traces/433.2809.addr
trace_file_name = traces/450.1208.addr
trace_file_name = traces/450.1208.addr
trace_file_name = traces/450.2193.addr
trace_file_name = traces/450.2193.addr
trace_file_name = traces/450.3514.addr
trace_file_name = traces/450.3514.addr
trace_file_name = traces/450.1824.addr
trace_file_name = traces/450.1824.addr
trace_file_name = traces/459.4929.addr
trace_file_name = traces/459.27331.addr
trace_file_name = traces/459.20380.addr
trace_file_name = traces/459.28777.addr
trace_file_name = traces/459.28777.addr
trace_file_name = traces/459.13843.addr
trace_file_name = traces/459.13843.addr
trace_file_name = traces/459.13843.addr
trace_file_name = traces/470.1391.addr
trace_file_name = traces/470.6035.addr
trace_file_name = traces/470.6035.addr
trace_file_name = traces/470.13190.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/433.4002.addr
trace_file_name = traces/433.4002.addr
trace_file_name = traces/433.88.addr
trace_file_name = traces/433.88.addr
trace_file_name = traces/433.9102.addr
trace_file_name = traces/433.1664.addr
trace_file_name = traces/433.2809.addr
trace_file_name = traces/433.2809.addr
trace_file_name = traces/450.1208.addr
trace_file_name = traces/450.1208.addr
trace_file_name = traces/450.2193.addr
trace_file_name = traces/450.2193.addr
trace_file_name = traces/450.3514.addr
trace_file_name = traces/450.3514.addr
trace_file_name = traces/450.1824.addr
trace_file_name = traces/450.1824.addr
trace_file_name = traces/459.4929.addr
trace_file_name = traces/459.27331.addr
trace_file_name = traces/459.20380.addr
trace_file_name = traces/459.28777.addr
trace_file_name = traces/459.28777.addr
trace_file_name = traces/459.13843.addr
trace_file_name = traces/459.13843.addr
trace_file_name = traces/459.13843.addr
trace_file_name = traces/470.1391.addr
trace_file_name = traces/470.6035.addr
trace_file_name = traces/470.6035.addr
trace_file_name = traces/470.13190.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/433.4002.addr
trace_file_name = traces/433.4002.addr
trace_file_name = traces/433.88.addr
trace_file_name = traces/433.88.addr
trace_file_name = traces/433.9102.addr
trace_file_name = traces/433.1664.addr
trace_file_name = traces/433.2809.addr
trace_file_name = traces/433.2809.addr
trace_file_name = traces/450.1208.addr
trace_file_name = traces/450.1208.addr
trace_file_name = traces/450.2193.addr
trace_file_name = traces/450.2193.addr
trace_file_name = traces/450.3514.addr
trace_file_name = traces/450.3514.addr
trace_file_name = traces/450.1824.addr
trace_file_name = traces/450.1824.addr
trace_file_name = traces/459.4929.addr
trace_file_name = traces/459.27331.addr
trace_file_name = traces/459.20380.addr
trace_file_name = traces/459.28777.addr
trace_file_name = traces/459.28777.addr
trace_file_name = traces/459.13843.addr
trace_file_name = traces/459.13843.addr
trace_file_name = traces/459.13843.addr
trace_file_name = traces/470.1391.addr
trace_file_name = traces/470.6035.addr
trace_file_name = traces/470.6035.addr
trace_file_name = traces/470.13190.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/470.7689.addr
trace_file_name = traces/470.7689.addr


