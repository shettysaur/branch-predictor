//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.hpp"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//

map<uint32_t, uint8_t> gBht;
map<uint32_t, pair<uint32_t,uint8_t> > lBht;
map<uint32_t, uint8_t> tournamentSelector;
uint32_t gHistory;
uint32_t gMask;
uint32_t lMask;
uint32_t pcIndexMask;
uint32_t addr;
uint32_t pcIndex;
uint32_t pcTags;
uint8_t localPrediction;
uint8_t globalPrediction;

void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  gBht.clear();
  lBht.clear();
  gHistory = 0;
  gMask = ((1 << (ghistoryBits))-1);
  lMask = ((1 << (lhistoryBits))-1);
  pcIndexMask = ((1 << (pcIndexBits+1))-1);
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      if(gBht.find(pc) != gBht.end()){
        return gBht[pc];
      }
      else{
        return NOTTAKEN;
      }
    case TWOBIT:
      if(gBht.find(pc) != gBht.end()){
        if(gBht[pc] == SN || gBht[pc] == WN)
          return NOTTAKEN;
        else
          return TAKEN;
      }
      else{
        gBht[pc] = SN;
        return NOTTAKEN;
      }
    case GSHARE:
		addr = (gHistory ^ pc);
		if(gBht.find(addr) != gBht.end()){
			if(gBht[addr] == SN || gBht[addr] == WN)
				return NOTTAKEN;
			else
				return TAKEN;
			}
		else{
			gBht[addr] = SN;
			return NOTTAKEN;
		}
    case TOURNAMENT:
		pcIndex = (pc & pcIndexMask);
		pcTags = (pc & ~pcIndexMask);
		if(lBht.find(pcIndex) != lBht.end()){
			if(lBht[pcIndex].first == pcTags){
				if(lBht[pcIndex].second == SN || lBht[pcIndex].second == WN){
					localPrediction = NOTTAKEN;
				}
				else{
					localPrediction = TAKEN;
				}
			}
			else{
				lBht[pcIndex]= make_pair(pcTags, SN);
				localPrediction = NOTTAKEN;
			}
		}
		else{
			lBht[pcIndex] = make_pair(pcTags, SN);
			localPrediction =  NOTTAKEN;
		}
		addr = (gHistory ^ pc);
		if(gBht.find(addr) != gBht.end()){
			if(gBht[addr] == SN || gBht[addr] == WN)
				globalPrediction = NOTTAKEN;
			else
				globalPrediction = TAKEN;
		}
		else{
			gBht[addr] = SN;
			globalPrediction = NOTTAKEN;
		}
		if(tournamentSelector.find(pcIndex) != tournamentSelector.end()){
			if(tournamentSelector[pcIndex] == SL || tournamentSelector[pcIndex] == WL){
				return localPrediction;
			}
			else{
				return globalPrediction;
			}
		}
		else{
			tournamentSelector[pcIndex] = SL;
			return localPrediction;
		}
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

  switch (bpType) {
    case STATIC:
		gBht[pc] = outcome;
    case TWOBIT:
		if(outcome){
			if(gBht[pc] != ST)
				gBht[pc]++;
			}
			else{
				if(gBht[pc] != SN)
					gBht[pc]--;
			}
    case GSHARE:
		if(outcome){
			if(gBht[addr] != ST)
				gBht[addr]++;
		}
		else{
			if(gBht[addr] != SN)
				gBht[addr]--;
		}
    case TOURNAMENT:
		if(outcome){
			if(gBht[addr] != ST)
				gBht[addr]++;
			if(lBht[pcIndex].second != ST)
				lBht[pcIndex].second++;
		}
		else{
			if(gBht[addr] != SN)
				gBht[addr]--;
			if(lBht[pcIndex].second != SN)
				lBht[pcIndex].second--;
		}
		if(localPrediction != globalPrediction){
			if(outcome == localPrediction){
				if(tournamentSelector[pcIndex] != SL)
					tournamentSelector[pcIndex]++;
			}
			else{
				if(tournamentSelector[pcIndex] != SG)
					tournamentSelector[pcIndex]--;
			}
		}
    case CUSTOM:
    default:
      break;
  }
  gHistory = ((gHistory << 1) | outcome) & gMask;
}
