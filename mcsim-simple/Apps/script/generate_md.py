#!/usr/bin/python

from optparse import OptionParser
import os, sys, re, string, fpformat, math

usage  = "usage: %prog [options]"
parser = OptionParser(usage)

parser.add_option("--infile", action="store", type="string", default="in.py", dest="infile",
                  help="input machine description file name (default = in.py)")
parser.add_option("--outprefix", action="store", type="string", default="out", dest="outprefix",
                  help="output machine description file prefix (default = out)")
parser.add_option("--tracefile", action="store", type="string", default="/dev/null", dest="tracefile",
                  help="input trace file name (default = /dev/null)")
(options, args) = parser.parse_args()

class MD:
  def __init__(self, filename):
    self.params = { }
    self.keys   = [ ]

    # parse mdfile
    try:
      mdfile = open(filename, 'r')
    except IOError:
      print "cannot open "+filename
      sys.exit()

    for line in mdfile:
      line = re.sub('#.*', '', line)  # remove comments
      temp = re.split('\s*', line)
      if len(temp) >= 3:
        if re.search("\^", temp[2]) != None:
          temp2 = re.split('\^', temp[2])
          self.params[temp[0]] = (int(temp2[0]) ** int(temp2[1]))
        else:
          self.params[temp[0]] = temp[2]
        self.keys.append(temp[0])
      else:
        self.keys.append("")



md = MD(options.infile)
traces = []

if options.tracefile != '/dev/null':
  try:
    tracefile = open(options.tracefile, 'r')
  except IOError:
    print "cannot open "+tracefile
    sys.exit()

  for line in tracefile:
    traces.append(line)


# generate following md files
# - (1, 2, 4, 8) vmds, (2, 4, 8) mini-ranks
# - (1, 2, 4) ranks
# - open vs. close
# - Chipkill 1 == (1 vmd, 2 ranks, close, 64KB page)
# - Chipkill 2 == (2 vmd, 4 ranks, close, 16KB page)
# so total 7*3*2 + 2 = 44 configurations

num_ranks    = [ 1, 2, 4 ]
mas_policies = [ "open", "closed" ]
num_vmds     = [ 1, 2, 4, 8 ]
mini_ranks   = [ "", "-minirank", "-minirank-BLlimit" ]

mc_process_bw = int(md.params["pts.mc.process_bw"])
mc_to_dir_t   = int(md.params["pts.mc.to_dir_t"])
mc_interleave_base_bit = int(md.params["pts.mc.interleave_base_bit"])
mc_interleave_xor_base_bit = int(md.params["pts.mc.interleave_xor_base_bit"])
mc_num_ranks_per_mc        = int(md.params["pts.mc.num_ranks_per_mc"])
mc_num_vmds_per_rank       = int(md.params["pts.mc.num_vmds_per_rank"])
mc_tBL        = int(md.params["pts.mc.tBL"])
mc_rank_interleave_base_bit = int(md.params["pts.mc.rank_interleave_base_bit"])
mc_vmd_interleave_base_bit  = int(md.params["pts.mc.vmd_interleave_base_bit"])
mc_bank_interleave_base_bit = int(md.params["pts.mc.bank_interleave_base_bit"])
mc_vmd_page_sz              = int(md.params["pts.mc.vmd_page_sz"])

if mc_num_ranks_per_mc != 1 or mc_num_vmds_per_rank != 1:
  print "use a proper md file"
  sys.exit()

for mas_policy in mas_policies:
  for num_rank in num_ranks:
    num_rank_log2 = int(math.log(num_rank, 2))
    for num_vmd in num_vmds:
      num_vmd_log2 = int(math.log(num_vmd, 2))
      for mini_rank in mini_ranks:
        if mini_rank != "" and num_vmd == 1:
          continue
        try:
          outfile = open(options.outprefix+'-vmd'+str(num_vmd)+'-rank'+str(num_rank)+'-'+mas_policy+mini_rank+'.py', 'w')
        except IOError:
          print "cannot open "+ outfile
          sys.exit()

        for key in md.keys:
          if key == "":
            outfile.write('\n')
            continue
          new_num = md.params[key]
          if key == "pts.mc.to_dir_t":
            new_num = mc_to_dir_t + (num_vmd - 1)*mc_process_bw*mc_tBL
          elif key == "pts.mc.interleave_base_bit":
            new_num = mc_interleave_base_bit - num_vmd_log2
          elif key == "pts.mc.interleave_xor_base_bit":
            new_num = mc_interleave_xor_base_bit + num_rank_log2
          elif key == "pts.mc.tBL":
            new_num = mc_tBL * num_vmd
          elif key == "pts.mc.rank_interleave_base_bit":
            new_num = mc_rank_interleave_base_bit - num_vmd_log2
          elif key == "pts.mc.vmd_interleave_base_bit":
            new_num = mc_rank_interleave_base_bit - num_vmd_log2 + num_rank_log2
          elif key == "pts.mc.bank_interleave_base_bit":
            new_num = mc_rank_interleave_base_bit + num_rank_log2
          elif key == "pts.mc.vmd_page_sz":
            new_num = mc_vmd_page_sz/num_vmd
          elif key == "pts.mc.num_ranks_per_mc":
            new_num = num_rank
          elif key == "pts.mc.num_vmds_per_rank":
            new_num = num_vmd
          elif key == "pts.mc.scheduling_policy":
            new_num = mas_policy
          elif key == "pts.mc.mini_rank":
            if mini_rank == "":
              new_num = 'false'
            elif mini_rank == "-minirank":
              new_num = 'true'
            else:
              new_num = 'true'
              outfile.write('pts.mc.tBL_of_DRAM = 4\n')
          outfile.write('%-40s = %s\n' % (key, str(new_num)))

        outfile.write('print_md = true\n')
        if options.tracefile != '/dev/null':
          outfile.write('\npts.use_trace_files = true\n')
          for trace in traces:
            outfile.write(trace)

