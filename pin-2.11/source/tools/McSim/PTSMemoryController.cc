/*
 * Copyright (c) 2010 The Hewlett-Packard Development Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Jung Ho Ahn
 */

#include "PTSMemoryController.h"
#include "PTSXbar.h"
#include "PTSDirectory.h"
#include <iomanip>

using namespace PinPthread;


ostream & operator << (ostream & output, mc_bank_action action)
{
  switch (action)
  {
    case mc_bank_activate:  output << "activate"; break;
    case mc_bank_read:      output << "read"; break;
    case mc_bank_write:     output << "write"; break;
    case mc_bank_precharge: output << "precharge"; break;
    case mc_bank_idle:      output << "idle"; break;
    default: break;
  }
  return output;
}

ostream & operator << (ostream & output, mc_scheduling_policy policy)
{
  switch (policy)
  {
    case mc_scheduling_open:   output << "open"; break;
    case mc_scheduling_closed: output << "closed"; break;
    default: break;
  }
  return output;
}

MemoryController::MemoryController(
    component_type type_,
    uint32_t num_,
    McSim * mcsim_)
:Component(type_, num_, mcsim_),
  req_l(),
  mc_to_dir_t  (get_param_uint64("to_dir_t", 1000)),
  num_ranks_per_mc (get_param_uint64("num_ranks_per_mc", 1)),
  num_banks_per_rank(get_param_uint64("num_banks_per_rank", 8)),
  tRCD         (get_param_uint64("tRCD", 10)),
  tRR          (get_param_uint64("tRR",  5)),
  tRP          (get_param_uint64("tRP",  tRP)),
  tCL          (get_param_uint64("tCL",  10)),
  tBL          (get_param_uint64("tBL",  10)),
  tBBL         (get_param_uint64("tBBL", tBL)),
  tRAS         (get_param_uint64("tRAS", 15)),
  tWRBUB       (get_param_uint64("tWRBUB", 2)),
  tRWBUB       (get_param_uint64("tRWBUB", 2)),
  tRRBUB       (get_param_uint64("tRRBUB", 2)),
  tWTR         (get_param_uint64("tWTR", 8)),
  req_window_sz(get_param_uint64("req_window_sz", 16)),
  mc_to_dir_t_ab(get_param_uint64("mc_to_dir_t_ab", mc_to_dir_t)),
  //read and set NVM members
  NVM_addr (get_param_uint64("NVM_addr", 200)),
  NVM_lat (get_param_uint64("NVM_lat", 200)),
  ///////////////
  tRCD_ab         (get_param_uint64("tRCD_ab", tRCD)),
  tRAS_ab         (get_param_uint64("tRAS_ab", tRAS)),
  tRP_ab          (get_param_uint64("tRP_ab",  tRP)),
  tCL_ab          (get_param_uint64("tCL_ab",  tCL)),
  num_banks_per_rank_ab(get_param_uint64("num_banks_per_rank_ab", 0)),
  rank_interleave_base_bit(get_param_uint64("rank_interleave_base_bit", 14)),
  bank_interleave_base_bit(get_param_uint64("bank_interleave_base_bit", 14)),
  page_sz_base_bit        (get_param_uint64("page_sz_base_bit", 12)),
  mc_interleave_base_bit(get_param_uint64("interleave_base_bit", 12)),
  num_mcs      (get_param_uint64("num_mcs", "pts.", 2)),
  num_read(0), num_write(0), num_activate(0), num_precharge(0),
  num_ab_read(0), num_ab_write(0), num_ab_activate(0),
  num_write_to_read_switch(0), num_refresh(0),
  bank_status(num_ranks_per_mc, vector<BankStatus>(num_banks_per_rank + num_banks_per_rank_ab)),
  last_activate_time(num_ranks_per_mc, 0),
  last_write_time(num_ranks_per_mc, 0),
  last_read_time(pair<uint32_t, uint64_t>(0, 0)),
  is_last_time_write(num_ranks_per_mc, false),
  dp_status(),
  rd_dp_status(),
  wr_dp_status()
{
  //cout << "NVM parameters:\n" << "addr = " << NVM_addr << endl;
  //cout << "latency = " << NVM_lat << endl;

  process_interval = get_param_uint64("process_interval", 10);
  refresh_interval = get_param_uint64("refresh_interval", 80000);
  next_refresh_time = refresh_interval;
  //geq->add_event(refresh_interval, this);
  curr_refresh_page = 0;
  curr_refresh_rank = 0;
  num_pages_per_bank = get_param_uint64("num_pages_per_bank", 8192);
  num_cached_pages_per_bank = get_param_uint64("num_cached_pages_per_bank", 4);
  interleave_xor_base_bit = get_param_uint64("interleave_xor_base_bit", 20);

  if (get_param_str("scheduling_policy") == "open")
  {
    policy = mc_scheduling_open;
  }
  else
  {
    policy = mc_scheduling_closed;
  }

  num_rw_interval  = 0;
  num_conflict_interval = 0;

  // variables below are used to find page_num quickly
  multimap<uint32_t, uint32_t> interleavers;
  interleavers.insert(pair<uint32_t, uint32_t>(rank_interleave_base_bit, num_ranks_per_mc));
  interleavers.insert(pair<uint32_t, uint32_t>(bank_interleave_base_bit, num_banks_per_rank));
  interleavers.insert(pair<uint32_t, uint32_t>(mc_interleave_base_bit,   num_mcs));

  multimap<uint32_t, uint32_t>::iterator iter = interleavers.begin();
  base2 = iter->first; width2 = iter->second; ++iter;
  base1 = iter->first; width1 = iter->second; ++iter;
  base0 = iter->first; width0 = iter->second; ++iter;

  par_bs      = get_param_str("par_bs")      == "true" ? true : false;
  full_duplex = get_param_str("full_duplex") == "true" ? true : false;

  is_fixed_latency      = get_param_str("is_fixed_latency") == "true" ? true : false;

  interleave_base_bit   = get_param_uint64("interleave_base_bit", 12);
  display_os_page_usage = get_param_str("display_os_page_usage") == "true" ? true : false;
  num_reqs              = 0;
  last_time_from_ab     = true;
  num_banks_with_agile_row = get_param_uint64("num_banks_with_agile_row", 0);
  reciprocal_of_agile_row_portion = get_param_uint64("reciprocal_of_agile_row_portion", 1);

  curr_batch_last       = -1;
  num_hthreads          = mcsim_->get_num_hthreads();
  num_req_from_a_th     = new int32_t[num_hthreads];
  for (uint32_t i = 0; i < num_hthreads; i++)
  {
    num_req_from_a_th[i] = 0;
  }
}


MemoryController::~MemoryController()
{
  if (num_read > 0)
  {
    update_acc_dist();

    cout << "  -- MC  [" << setw(3) << num << "] : (rd, wr, act, pre) = ("
         << setw(9) << num_read << ", " << setw(9) << num_write << ", "
         << setw(9) << num_activate << ", " << setw(9) << num_precharge
         << "), # of WR->RD switch = " << num_write_to_read_switch
         << ", #_refresh = " << num_refresh << ", "
         << os_page_acc_dist.size() << " pages acc, AB (rd, wr, act) = ("
         << setw(9) << num_ab_read << ", " << setw(9) << num_ab_write << ", "
         << setw(9) << num_ab_activate << ")" << endl;
  }

  if (display_os_page_usage == true)
  {
    for(map<uint64_t, uint64_t>::iterator iter = os_page_acc_dist.begin(); iter != os_page_acc_dist.end(); ++iter)
    {
      cout << "  -- page 0x" << setfill('0') << setw(8) << hex << iter->first * (1 << interleave_base_bit)
           << setfill(' ') << dec << " is accessed (" 
           << setw(7) << mcsim->os_page_req_dist[iter->first]
           << ", " << setw(7) << iter->second << ") times at (Core, MC)." <<  endl;
    }
  }
}


void MemoryController::add_req_event(
    uint64_t event_time,
    LocalQueueElement * local_event,
    Component * from)
{
  //print out the event type
  if ((local_event-> type) != et_e_rd) 
    cout << "--------------------------- event type = " << local_event -> type << endl;


  if (event_time % process_interval != 0)
  { // make the new event_time % process_interv == 0
    event_time += process_interval - event_time%process_interval;
  }

  if (is_fixed_latency == true)
  {
    //cout << "is_fixed_lat\n";
    if (local_event->type == et_evict || local_event->type == et_dir_evict)
    {
      delete local_event;
    }
    else
    {
      //add additional latency for NVM
      //if (local_event->address > NVM_addr)
        //event_time += NVM_lat;
        //cout << "mc to dir time = " << mc_to_dir_t << endl;
        //cout << "event time = " << event_time << endl;
      //cout << "===============  NVM_addr = " << NVM_addr;
      //cout << "========== NVM_lat = " << NVM_lat << endl;
      directory->add_rep_event(event_time + mc_to_dir_t, local_event);
    }
  }
  else
  {

    geq->add_event(event_time, this);
    req_event.insert(pair<uint64_t, LocalQueueElement *>(event_time, local_event));
    check_bank_status(local_event);
  }

  // update access distribution
  num_reqs++;
  uint64_t page_num = local_event->address / (1 << interleave_base_bit);
  map<uint64_t, uint64_t>::iterator p_iter = os_page_acc_dist_curr.find(page_num);

  if (p_iter != os_page_acc_dist_curr.end())
  {
    (p_iter->second)++;
  }
  else
  {
    os_page_acc_dist_curr.insert(pair<uint64_t, uint64_t>(page_num, 1));
  }
  
  /*if (event_time > 50000000 && event_time < 50500000)
  {
    uint64_t address  = local_event->address;
    uint32_t rank_num = ((address >> rank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_ranks_per_mc;
    uint32_t bank_num = ((address >> bank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_banks_per_rank;
    cout << setw(10) << event_time << "  -- (" << rank_num << ", " << bank_num
         << ", 0x" << hex << get_page_num(address) << dec << ") : " << " : "; local_event->display();
  }*/
}


void MemoryController::add_rep_event(
    uint64_t event_time,
    LocalQueueElement * local_event,
    Component * from)
{
  add_req_event(event_time, local_event, from);
}


uint64_t MemoryController::get_page_num(uint64_t addr)
{
  uint64_t page_num = addr;

  page_num = (((page_num >> base0) / width0) << base0) + (page_num % (1 << base0));
  page_num = (((page_num >> base1) / width1) << base1) + (page_num % (1 << base1));
  page_num = (((page_num >> base2) / width2) << base2) + (page_num % (1 << base2));

  return (page_num >> page_sz_base_bit);
}


void MemoryController::show_state(uint64_t curr_time)
{
  cout << "  -- MC  [" << num << "] : curr_time = " << curr_time << endl;

  uint32_t i;
  vector<LocalQueueElement *>::iterator iter;
  for (iter = req_l.begin(), i = 0; iter != req_l.end()/* && i < req_window_sz*/ ; ++iter, ++i)
  {
    uint64_t address  = (*iter)->address;
    uint32_t rank_num = ((address >> rank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_ranks_per_mc;
    uint32_t bank_num = ((address >> bank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_banks_per_rank;
    cout << "  -- req_l[" << setw(2) <<  i << "] = (" << rank_num << ", " << bank_num
         << ", 0x" << hex << get_page_num((*iter)->address) << dec << ") : " << hex << (uint64_t *)(*iter) << dec << " : "; (*iter)->display();
  }
  if (curr_batch_last >= 0)
  {
    cout << "  -- current batch ends at req_l[" << setw(2) << curr_batch_last << "]" << endl;
  }

  for (uint32_t i = 0; i < num_ranks_per_mc; i++)
  {
    for(uint32_t k = 0; k < num_banks_per_rank + num_banks_per_rank_ab; k++)
    {
      cout << "  -- bank_status[" << setw(2) <<  i << "][" << setw(2) << k << "] = ("
           << setw(10) << bank_status[i][k].action_time << ", "
           << hex << "0x" << bank_status[i][k].page_num << dec << ", "
           << bank_status[i][k].action_type << ")" << endl;
    }
  }

  for (uint32_t i = 0; i < num_ranks_per_mc; i++)
  {
    cout << "  -- last_activate_time[" << i << "] = " << last_activate_time[i] << endl;
  }


  map<uint64_t, mc_bank_action>::iterator miter = dp_status.begin();

  while (miter != dp_status.end())
  {
    cout << "  -- data_path_status = ("
         << miter->first << ", "
         << miter->second << ")" << endl;
    ++miter;
  }
  miter = rd_dp_status.begin();

  while (miter != rd_dp_status.end())
  {
    cout << "  -- rd_dp_status = ("
         << miter->first << ", "
         << miter->second << ")" << endl;
    ++miter;
  }
  miter = wr_dp_status.begin();

  while (miter != wr_dp_status.end())
  {
    cout << "  -- wr_dp_status = ("
         << miter->first << ", "
         << miter->second << ")" << endl;
    ++miter;
  }
}


uint32_t MemoryController::process_event(uint64_t curr_time)
{

  bool command_sent = pre_processing(curr_time);
  uint32_t i, i2;
  vector<LocalQueueElement *>::iterator iter, iter2;

  //// print out event type
  //if ((*iter)-> type != et_e_rd) 
    //cout << "---------------------------  event type = " << (*iter)-> type<< endl;

  /*if (curr_time % refresh_interval == 0)
    {
    geq->add_event(curr_time + refresh_interval, this);
    geq->add_event(curr_time + process_interval, this);
    num_refresh++;
    command_sent      = true;
    curr_refresh_rank = (curr_refresh_rank + 1)%num_ranks_per_mc;
    curr_refresh_page = (curr_refresh_page + ((curr_refresh_rank == 0) ? 1: 0))%num_pages_per_bank;

    for (uint32_t j = 0; j < num_banks_per_rank; j++)
    {
    BankStatus & curr_bank = bank_status[curr_refresh_rank][i][j];
    curr_bank.action_type  = mc_bank_precharge;
    curr_bank.action_time  = curr_time + (((tRCD > tCL) ? tRCD : tCL) + tRP + tRCD + tRP)*process_interval;
    curr_bank.page_num = curr_refresh_page;
    }
    }*/

  int32_t c_idx    = -1;                                       // candidate index
  vector<LocalQueueElement *>::iterator c_iter = req_l.end();
  bool    page_hit = false;
  int32_t num_req_from_the_same_thread = req_l.size() + 1;
  // first, find a request that can be serviced at this cycle 
  for (int32_t h = 0; h < (num_banks_per_rank_ab > 0 ? 2 : 1); h++)
  {
    /*if (curr_time > 50000000 && curr_time < 50050000)
    {
      //cout << setw(10) << curr_time << "   " << req_l.size() << endl;
      //show_state(curr_time);
    }*/
      
  for (iter = req_l.begin(), i = 0; iter != req_l.end()/* && i < req_window_sz*/; ++iter, ++i)
  {
    if (c_idx >= 0 && iter != req_l.begin() && (int32_t)i > curr_batch_last)
    {
      // we found a candidate from the ready batch already
      break;
    }

    if (num_banks_per_rank_ab > 0)
    {
      if ((h == 0 && last_time_from_ab == true   && i >= req_window_sz) ||
          (h == 1 && last_time_from_ab == false  && i >= req_window_sz))
      {
        break;
      }
    }
    else
    {
      if (i >= req_window_sz)
      {
        break;
      }
    }

    // check constraints
    uint64_t address  = (*iter)->address;


    if (num_banks_per_rank_ab > 0)
    {
      if ((h == 0 && (address >> 63 != 0) && last_time_from_ab == true) ||
          (h == 1 && (address >> 63 == 0) && last_time_from_ab == true) ||
          (h == 0 && (address >> 63 == 0) && last_time_from_ab == false) ||
          (h == 1 && (address >> 63 != 0) && last_time_from_ab == false))
      {
        continue;
      }
    }
    uint32_t num_banks_per_rank_curr = (address >> 63 != 0 && num_banks_per_rank_ab != 0) ? num_banks_per_rank_ab : (address >> 63 != 0 && reciprocal_of_agile_row_portion == 0 ? num_banks_with_agile_row : num_banks_per_rank);
    event_type type   = (*iter)->type;
    uint32_t rank_num = ((address >> rank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_ranks_per_mc;
    uint32_t bank_num = ((address >> bank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_banks_per_rank_curr + ((address >> 63 != 0 && num_banks_per_rank_ab != 0) ? num_banks_per_rank : 0);
    uint64_t page_num = get_page_num(address);

    bool access_agile = (address >> 63 != 0 ||
                         (bank_num < num_banks_with_agile_row && reciprocal_of_agile_row_portion != 0 && page_num%reciprocal_of_agile_row_portion == 0));
    uint32_t tRCD_curr        = (access_agile) ? tRCD_ab : tRCD;
    uint32_t tRP_curr         = (access_agile) ? tRP_ab  : tRP;

    //cout << hex << page_num << dec << "  " << access_agile << "  " << page_num%reciprocal_of_agile_row_portion << "  " << bank_num << "  " << hex << address << " " << (address >> bank_interleave_base_bit) << " " << (address >> interleave_xor_base_bit) << endl;
  
    BankStatus & curr_bank = bank_status[rank_num][bank_num];
    map<uint64_t, mc_bank_action>::iterator miter, rd_miter, wr_miter;

    if (curr_bank.action_time > curr_time)
    {
      continue;
    }
    switch (curr_bank.action_type)
    {
      case mc_bank_precharge:
        if (curr_bank.action_time + tRP_curr*process_interval > curr_time)
        {
          break;
        }
      case mc_bank_idle:
        if (page_hit == false &&  // page_hit has priority 2
            last_activate_time[rank_num] + tRR*process_interval <= curr_time)
        {
          if (num_req_from_a_th[(*iter)->th_id] < num_req_from_the_same_thread ||
              (num_req_from_a_th[(*iter)->th_id] == num_req_from_the_same_thread && c_idx == -1))
          {
            c_idx  = i;
            c_iter = iter;
            num_req_from_the_same_thread = num_req_from_a_th[(*iter)->th_id];
          }
        }
        break;
      case mc_bank_activate:
        if (curr_bank.action_time + tRCD_curr*process_interval > curr_time)
        {
          break;
        }
      case mc_bank_read:
      case mc_bank_write:
        if (curr_bank.action_time + tBBL*process_interval > curr_time)
        {
          break;
        }
        if (curr_bank.page_num != page_num)
        { // row miss
          if (policy == mc_scheduling_open)
          {
            uint32_t k = 0;
            bool need_precharge = true;
            iter2 = req_l.begin();

            while (iter2 != req_l.end() && k++ < req_window_sz)
            {
              if ((int32_t)i <= curr_batch_last && (int32_t)k > curr_batch_last)
              {
                break;
              }
              if (iter == iter2) 
              {
                iter2++;
                continue;
              }
              uint64_t address  = (*iter2)->address;
              uint32_t num_banks_per_rank_curr = (address >> 63 != 0 && num_banks_per_rank_ab != 0) ? num_banks_per_rank_ab : (address >> 63 != 0 && reciprocal_of_agile_row_portion == 0 ? num_banks_with_agile_row : num_banks_per_rank);
              if (rank_num == ((address >> rank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_ranks_per_mc &&
                  bank_num == ((address >> bank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_banks_per_rank_curr + ((address >> 63 != 0 && num_banks_per_rank_ab != 0) ? (address >> 63 != 0 && reciprocal_of_agile_row_portion == 0 ? num_banks_with_agile_row : num_banks_per_rank) : 0)  &&
                  curr_bank.page_num == get_page_num(address))
              {
                need_precharge = false;
                break;
              }
              iter2++;
            }

            if (need_precharge == true &&
                page_hit == false &&  // page_hit has priority 2
                last_activate_time[rank_num] + tRR*process_interval <= curr_time)
            {
              if (num_req_from_a_th[(*iter)->th_id] < num_req_from_the_same_thread ||
                  (num_req_from_a_th[(*iter)->th_id] == num_req_from_the_same_thread && c_idx == -1))
              {
                c_idx  = i;
                c_iter = iter;
                num_req_from_the_same_thread = num_req_from_a_th[(*iter)->th_id];
              }
            }
          }
        }
        else
        { // row hit
          dp_status.erase(      dp_status.begin(),    dp_status.lower_bound(curr_time));
          miter    =    dp_status.lower_bound(curr_time + tCL*process_interval);
          bool met_constraints = false;
          switch (type)
          {
            case et_rd_dir_info_req:
            case et_rd_dir_info_rep:
            case et_read:
            case et_e_rd:
            case et_s_rd:
              // read
              rd_dp_status.erase(rd_dp_status.begin(), rd_dp_status.lower_bound(curr_time));
              rd_miter = rd_dp_status.lower_bound(curr_time + tCL*process_interval);
              if ((full_duplex == false && 
                    (miter == dp_status.end() || miter->first >= curr_time + (tCL+tBL)*process_interval)) ||
                  (full_duplex == true &&
                   (rd_miter == rd_dp_status.end() || rd_miter->first >= curr_time + (tCL+tBL)*process_interval)))
              {
                bool wrbub = false;
                // WRBUB
                miter = dp_status.lower_bound(curr_time + tCL*process_interval - tWRBUB*process_interval);
                while (full_duplex == false &&
                    miter != dp_status.end() &&
                    miter->first < curr_time + tCL*process_interval)
                {
                  if (miter->second == mc_bank_write)
                  {
                    wrbub = true;
                    break;
                  }
                  ++miter;
                }

                if (wrbub == false && tWTR > 0 && last_write_time[rank_num] + tWTR*process_interval > curr_time)
                {  // tWTR constraint
                  wrbub = true;
                }

                if (wrbub == false && last_read_time.first != rank_num &&
                    curr_time < tRRBUB*process_interval + last_read_time.second)
                {  // tRRBUB
                  wrbub = true;
                }

                if (wrbub == false)
                { // service the read request
                  met_constraints = true;
                }
              }
              break;
            case et_evict:
            case et_dir_evict:
            case et_s_rd_wr:
              // write
              wr_dp_status.erase(wr_dp_status.begin(), wr_dp_status.lower_bound(curr_time));
              wr_miter = wr_dp_status.lower_bound(curr_time + tCL*process_interval);
              if ((full_duplex == false &&
                    (miter == dp_status.end() || miter->first >= curr_time + (tCL+tBL)*process_interval)) ||
                  (full_duplex == true  &&
                   (wr_miter == wr_dp_status.end() || wr_miter->first >= curr_time + (tCL+tBL)*process_interval)))
              {
                bool rwbub = false;
                // RWBUB
                miter = dp_status.lower_bound(curr_time + tCL*process_interval - tRWBUB*process_interval);
                while (full_duplex == false &&
                    miter != dp_status.end() &&
                    miter->first < curr_time + tCL*process_interval)
                {
                  if (miter->second == mc_bank_read)
                  {
                    rwbub = true;
                    break;
                  }
                  ++miter;
                }

                if (rwbub == false)
                { // service the write request
                  met_constraints = true;
                }
              }
              break;
            default:
              cout << "action_type = " << curr_bank.action_type << endl;
              show_state(curr_time);  ASSERTX(0);
              break;
          }
          if (met_constraints == true &&
              (page_hit == false ||  // this request is page_hit
               num_req_from_a_th[(*iter)->th_id] < num_req_from_the_same_thread))
          {
            c_idx  = i;
            c_iter = iter;
            page_hit = true;
            num_req_from_the_same_thread = num_req_from_a_th[(*iter)->th_id];
          }
        }
        break;
      default:
        cout << "curr (rank, bank) = (" << rank_num << ", " << bank_num << ")" << endl;
        show_state(curr_time);  ASSERTX(0);
        break;
    }
  }
  }

  if (c_idx >= 0)
  {
    i    = c_idx;
    iter = c_iter;
    // check bank_status
    uint64_t address  = (*iter)->address;
    uint32_t num_banks_per_rank_curr = (address >> 63 != 0 && num_banks_per_rank_ab != 0) ? num_banks_per_rank_ab : (address >> 63 != 0 && reciprocal_of_agile_row_portion == 0 ? num_banks_with_agile_row : num_banks_per_rank);

    event_type type   = (*iter)->type;
    uint32_t rank_num = ((address >> rank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_ranks_per_mc;
    uint32_t bank_num = ((address >> bank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_banks_per_rank_curr + ((address >> 63 != 0 && num_banks_per_rank_ab != 0) ? (address >> 63 != 0 && reciprocal_of_agile_row_portion == 0 ? num_banks_with_agile_row : num_banks_per_rank) : 0);
    uint64_t page_num = get_page_num(address);
    BankStatus & curr_bank = bank_status[rank_num][bank_num];

    bool access_agile = (address >> 63 != 0 ||
                         (bank_num < num_banks_with_agile_row && reciprocal_of_agile_row_portion != 0 && page_num%reciprocal_of_agile_row_portion == 0));
    uint32_t mc_to_dir_t_curr = (access_agile) ? mc_to_dir_t_ab : mc_to_dir_t;
    uint32_t tRAS_curr        = (access_agile) ? tRAS_ab : tRAS;
    uint32_t tCL_curr         = (access_agile) ? tCL_ab  : tCL;
    map<uint64_t, mc_bank_action>::iterator miter, rd_miter, wr_miter;

    switch (curr_bank.action_type)
    {
      case mc_bank_precharge:
      case mc_bank_idle:
        curr_bank.action_time  = curr_time;
        curr_bank.page_num = page_num;
        curr_bank.action_type  = mc_bank_activate;
        last_activate_time[rank_num] = curr_time;
        curr_bank.last_activate_time = curr_time;
        num_activate++;
        num_ab_activate += (access_agile) ? 1 : 0;
        break;
      case mc_bank_activate:
      case mc_bank_read:
      case mc_bank_write:
        if (curr_bank.page_num != page_num)
        {  // row miss
          if (policy == mc_scheduling_open)
          {
            command_sent = true;
            num_precharge++;
            curr_bank.action_time = (curr_time - curr_bank.last_activate_time >= tRAS_curr*process_interval) ?
              curr_time : (tRAS_curr*process_interval + curr_bank.last_activate_time);
            curr_bank.action_type = mc_bank_precharge;
          }
          break;
        }
        else
        {  // row hit
          last_time_from_ab = (address >> 63 != 0) ? true : false;
          switch (type)
          {
            case et_rd_dir_info_req:
            case et_rd_dir_info_rep:
            case et_read:
            case et_e_rd:
            case et_s_rd:
              // read
              if (is_last_time_write[rank_num] == true)
              {
                is_last_time_write[rank_num] = false;
                num_write_to_read_switch++;
              }
              num_read++;
              num_ab_read += (access_agile) ? 1 : 0;
              curr_bank.action_time = curr_time;

              if (last_time_from_ab == true)
              {
                tCL_curr = tCL;
                mc_to_dir_t_curr = mc_to_dir_t;
                miter    =    dp_status.lower_bound(curr_time + tCL_ab*process_interval);
                rd_miter = rd_dp_status.lower_bound(curr_time + tCL_ab*process_interval);
                if ((full_duplex == false && 
                      (miter == dp_status.end() || miter->first >= curr_time + (tCL_ab+tBL)*process_interval)) ||
                    (full_duplex == true &&
                     (rd_miter == rd_dp_status.end() || rd_miter->first >= curr_time + (tCL_ab+tBL)*process_interval)))
                {
                  tCL_curr = tCL_ab;
                  mc_to_dir_t_curr = mc_to_dir_t_ab;
                }
              }

              for (uint32_t j = 0; j < tBL; j++)
              {
                uint64_t next_time = curr_time + (tCL_curr+j)*process_interval;
                dp_status.insert   (pair<uint64_t, mc_bank_action>(next_time, mc_bank_read));
                rd_dp_status.insert(pair<uint64_t, mc_bank_action>(next_time, mc_bank_read));
              }
              last_read_time.first  = rank_num;
              last_read_time.second = curr_time;// + (tCL_curr + tBL)*process_interval;

              //add additional latency for NVM
              //if ((*iter)->address > NVM_addr)
                //curr_time += NVM_lat;
        //cout << "mc to dir time = " << mc_to_dir_t_curr << endl;
        //cout << "event time = " << curr_time << endl;
      //cout << "===============  NVM_addr = " << NVM_addr;
      //cout << "========== NVM_lat = " << NVM_lat << endl;
              directory->add_rep_event(curr_time + mc_to_dir_t_curr, *iter);
              if (par_bs == true) num_req_from_a_th[(*iter)->th_id]--;
              if (policy == mc_scheduling_open)
              {
                curr_bank.action_type = mc_bank_read;
              }
              else
              {
                curr_bank.action_time = (curr_time - curr_bank.last_activate_time >= tRAS_curr*process_interval) ?
                                        curr_time : (tRAS_curr*process_interval + curr_bank.last_activate_time);
                curr_bank.action_type = mc_bank_precharge;
                num_precharge++;
                iter2 = iter;
                i2    = i;
                ++iter2;
                ++i2;
                for ( ; iter2 != req_l.end() && i2 < req_window_sz; ++iter2, ++i2)
                {
                  uint64_t address  = (*iter2)->address;
                  uint32_t num_banks_per_rank_curr = (address >> 63 != 0 && num_banks_per_rank_ab != 0) ? num_banks_per_rank_ab : (address >> 63 != 0 && reciprocal_of_agile_row_portion == 0 ? num_banks_with_agile_row : num_banks_per_rank);
                  if (rank_num == ((address >> rank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_ranks_per_mc &&
                      bank_num == ((address >> bank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_banks_per_rank_curr + ((address >> 63 != 0 && num_banks_per_rank_ab != 0) ? (address >> 63 != 0 && reciprocal_of_agile_row_portion == 0 ? num_banks_with_agile_row : num_banks_per_rank) : 0)  &&
                      page_num == get_page_num(address))
                  {
                    curr_bank.action_type = mc_bank_read;
                    num_precharge--;
                    break;
                  }
                }
              }
              if (par_bs == true && curr_batch_last == (int32_t)i)
              {
                if (i == 0)
                {
                  curr_batch_last = (int32_t)req_l.size() - 2;
                }
                else
                {
                  curr_batch_last--;
                }
              }
              else if (par_bs == true && curr_batch_last > (int32_t)i)
              {
                curr_batch_last--;
              }
              req_l.erase(iter);
              break;
            case et_evict:
            case et_dir_evict:
            case et_s_rd_wr:
              // write
              is_last_time_write[rank_num] = true;
              num_write++;
              num_ab_write += (access_agile) ? 1 : 0;
              curr_bank.action_time = curr_time;
              
              if (last_time_from_ab == true)
              {
                tCL_curr = tCL;
                mc_to_dir_t_curr = mc_to_dir_t;
                miter    =    dp_status.lower_bound(curr_time + tCL_ab*process_interval);
                wr_miter = wr_dp_status.lower_bound(curr_time + tCL_ab*process_interval);
                if ((full_duplex == false && 
                      (miter == dp_status.end() || miter->first >= curr_time + (tCL_ab+tBL)*process_interval)) ||
                    (full_duplex == true &&
                     (wr_miter == wr_dp_status.end() || wr_miter->first >= curr_time + (tCL_ab+tBL)*process_interval)))
                {
                  tCL_curr = tCL_ab;
                  mc_to_dir_t_curr = mc_to_dir_t_ab;
                }
              }

              for (uint32_t j = 0; j < tBL; j++)
              {
                uint64_t next_time = curr_time + (tCL_curr+j)*process_interval;
                dp_status.insert   (pair<uint64_t, mc_bank_action>(next_time, mc_bank_write));
                wr_dp_status.insert(pair<uint64_t, mc_bank_action>(next_time, mc_bank_write));
              }
              last_write_time[rank_num] = curr_time + (tCL_curr+tBL)*process_interval;

              if (par_bs == true) num_req_from_a_th[(*iter)->th_id]--;
              if (type == et_s_rd_wr)
              {
                (*iter)->type = et_s_rd;
                //add additional latency for NVM
                //if ((*iter)->address > NVM_addr)
                  //curr_time += NVM_lat;
        //cout << "mc to dir time = " << mc_to_dir_t_curr << endl;
        //cout << "event time = " << curr_time << endl;
      //cout << "===============  NVM_addr = " << NVM_addr;
      //cout << "========== NVM_lat = " << NVM_lat << endl;
                directory->add_rep_event(curr_time + mc_to_dir_t_curr, *iter);
              }
              else
              {
                delete *iter;
              }
              if (policy == mc_scheduling_open)
              {
                curr_bank.action_type = mc_bank_write;
              }
              else
              {
                curr_bank.action_time = (curr_time - curr_bank.last_activate_time >= tRAS_curr*process_interval) ? 
                                        curr_time : (tRAS_curr*process_interval + curr_bank.last_activate_time);
                curr_bank.action_type = mc_bank_precharge;
                num_precharge++;
                iter2 = iter;
                i2    = i;
                ++iter2;
                ++i2;
                for ( ; iter2 != req_l.end() && i2 < req_window_sz; ++iter2, ++i2)
                {
                  uint64_t address  = (*iter2)->address;
                  uint32_t num_banks_per_rank_curr = (address >> 63 != 0 && num_banks_per_rank_ab != 0) ? num_banks_per_rank_ab : (address >> 63 != 0 && reciprocal_of_agile_row_portion == 0 ? num_banks_with_agile_row : num_banks_per_rank);
                  if (rank_num == ((address >> rank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_ranks_per_mc &&
                      bank_num == ((address >> bank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_banks_per_rank_curr + ((address >> 63 != 0 && num_banks_per_rank_ab != 0) ? (address >> 63 != 0 && reciprocal_of_agile_row_portion == 0 ? num_banks_with_agile_row : num_banks_per_rank) : 0)  &&
                      page_num == get_page_num(address))
                  {
                    curr_bank.action_type = mc_bank_write;
                    num_precharge--;
                    break;
                  }
                }
              }
              if (par_bs == true && curr_batch_last == (int32_t)i)
              {
                if (i == 0)
                {
                  curr_batch_last = (int32_t)req_l.size() - 2;
                }
                else
                {
                  curr_batch_last--;
                }
              }
              else if (par_bs == true && curr_batch_last > (int32_t)i)
              {
                curr_batch_last--;
              }
              req_l.erase(iter);
              break;
            default:
              cout << "action_type = " << curr_bank.action_type << endl;
              show_state(curr_time);  ASSERTX(0);
              break;
          }
        }
        break;
      default:
        cout << "curr (rank, bank) = (" << rank_num << ", " << bank_num << ")" << endl;
        show_state(curr_time);  ASSERTX(0);
        break;
    }
  }

  //show_state(curr_time);
  if (req_l.empty() == false)
  {
    geq->add_event(curr_time + process_interval, this);
  }

  return 0;
}


bool MemoryController::pre_processing(uint64_t curr_time)
{
  multimap<uint64_t, LocalQueueElement *>::iterator req_event_iter = req_event.begin();

  while (req_event_iter != req_event.end() && req_event_iter->first == curr_time)
  {
    if (par_bs == true)
    {
      num_req_from_a_th[req_event_iter->second->th_id]++;
    }
    req_l.push_back(req_event_iter->second);
    ++req_event_iter;
  }
  if (par_bs == true && curr_batch_last == -1 && req_l.size() > 0)
  {
    curr_batch_last = (int32_t)req_l.size() - 1;
  }
  req_event.erase(curr_time);


  bool command_sent = false;
  vector<LocalQueueElement *>::iterator iter, iter2;

  /*if (par_bs == true)
  {
    for (uint32_t i = 0; i < num_hthreads; i++)
    {
      num_req_from_a_th[i] = 0;
    }
    for (iter = req_l.begin(), i = 0; iter != req_l.end() && i < req_window_sz; ++iter, ++i)
    {
      num_req_from_a_th[(*iter)->th_id]++;
    }
  }*/

  return command_sent;
}


void MemoryController::update_acc_dist()
{
  map<uint64_t, uint64_t>::iterator p_iter, c_iter;

  for (c_iter = os_page_acc_dist_curr.begin(); c_iter != os_page_acc_dist_curr.end(); ++c_iter)
  {
    p_iter = os_page_acc_dist.find(c_iter->first);

    if (p_iter == os_page_acc_dist.end())
    {
      os_page_acc_dist.insert(pair<uint64_t, uint64_t>(c_iter->first, 1));
    }
    else
    {
      p_iter->second += c_iter->second;
    }
  }

  os_page_acc_dist_curr.clear();
}


void MemoryController::check_bank_status(LocalQueueElement * local_event)
{
  uint64_t address  = (local_event)->address;
  uint32_t num_banks_per_rank_curr = (address >> 63 != 0 && num_banks_per_rank_ab != 0) ? num_banks_per_rank_ab : (address >> 63 != 0 && reciprocal_of_agile_row_portion == 0 ? num_banks_with_agile_row : num_banks_per_rank);
  uint32_t rank_num = ((address >> rank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_ranks_per_mc;
  uint32_t bank_num = ((address >> bank_interleave_base_bit) ^ (address >> interleave_xor_base_bit)) % num_banks_per_rank_curr + ((address >> 63 != 0 && num_banks_per_rank_ab != 0) ? num_banks_per_rank : 0);
  uint64_t page_num = get_page_num(address);

  BankStatus & curr_bank = bank_status[rank_num][bank_num];

  if (page_num != curr_bank.page_num)
  {
    num_conflict_interval++;
  }
  num_rw_interval++;
}

