//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <math.h>
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
class Counter {       
  public:             
    int bits;
	int value;

	Counter(int bits){
		this->bits = bits;
		this->value = 0;
	}

	Counter(int bits, int value){
		this->bits = bits;
		this->value = value;
	}

	void increment(){
		if (this->value != ((1 << bits)-1))
			this->value++;
	}

	void decrement(){
		if (this->value != 0)
			this->value--;
	}
};

class SignedCounter {       
  public:             
    int bits;
	int value;

	SignedCounter(int bits){
		this->bits = bits;
		this->value = -(1 << (bits-1));
	}

	SignedCounter(int bits, int value){
		this->bits = bits;
		this->value = value;
	}

	void increment(){
		if (this->value != ((1 << (bits-1))-1))
			this->value++;
	}

	void decrement(){
		if (this->value != -(1 << (bits-1)))
			this->value--;
	}
};

map<uint32_t, SignedCounter*> gBht;
map<uint32_t, pair<uint32_t,uint32_t> > lBht;
map<uint32_t, uint8_t> localPredictionMap;
map<uint32_t, uint8_t> tournamentSelector;
uint32_t gHistory;
uint32_t lHistory;
uint32_t gMask;
uint32_t lMask;
uint32_t pcIndexMask;
uint32_t gIndex;
uint32_t lIndex;
uint32_t pcIndex;
uint32_t pcTags;
uint8_t localPrediction;
uint8_t globalPrediction;

//CUSTOM
map<uint32_t, SignedCounter*> gehl[M];
int S;
uint32_t gHistoryArray[M];
uint32_t gMaskArray[M];
uint8_t prediction;

int n = 0;
int counterBits = 3;

void init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  gBht.clear();
  lBht.clear();
  gHistory = 0;
  gMask = ((1 << (ghistoryBits))-1);
  lMask = ((1 << (lhistoryBits))-1);
  pcIndexMask = ((1 << (pcIndexBits))-1);

  uint32_t x = 2;
  for(int i = 0 ; i < M ; i++){
	  gMaskArray[i] = ((1 << (x))-1);
	  x *= 2;
  }
  
  //CUSTOM

}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      if(gBht.find(pc) != gBht.end()){
        return gBht[pc]->value;
      }
      else{
        return NOTTAKEN;
      }
    case TWOBIT:
      if(gBht.find(pc) != gBht.end()){
        if(gBht[pc]->value == SN || gBht[pc]->value == WN)
          return NOTTAKEN;
        else
          return TAKEN;
      }
      else{
        gBht.insert(make_pair(pc, new SignedCounter(counterBits)));
        return NOTTAKEN;
      }
    case GSHARE:
		return gshare_prediction(pc);
		
    case TOURNAMENT:
		localPrediction = local_prediction(pc);
		globalPrediction = gshare_prediction(pc);
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
		return gehl_prediction(pc);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

uint8_t gshare_prediction(uint32_t pc){
	gIndex = (gHistory ^ pc) & gMask;
	if(gBht.find(gIndex) != gBht.end()){
		// cout << "GIndex : " << gIndex << " predition : " << gBht[gIndex]->value << endl;
		if(gBht[gIndex]->value >= 0)
			return TAKEN;
		else
			return NOTTAKEN;
		}
	else{
		gBht.insert(make_pair(gIndex, new SignedCounter(counterBits)));
		return NOTTAKEN;
	}
}

uint8_t local_prediction(uint32_t pc){
	pcIndex = (pc & pcIndexMask);
	pcTags = (pc & ~pcIndexMask);
	if(lBht.find(pcIndex) != lBht.end()){
		if(lBht[pcIndex].first == pcTags){
			lHistory = lBht[pcIndex].second;
			lIndex = lHistory;
			if(localPredictionMap.find(lIndex) != localPredictionMap.end()){
				if(localPredictionMap[lIndex] == SN || localPredictionMap[lIndex] == WN){
					return NOTTAKEN;
				}
				else{
					return TAKEN;
				}
			}
			else{
				localPredictionMap[lIndex] = SN;
				return NOTTAKEN;
			}
		}
		else{
			lBht[pcIndex]= make_pair(pcTags, 0);
			return NOTTAKEN;
		}
	}
	else{
		lBht[pcIndex] = make_pair(pcTags, SN);
		return  NOTTAKEN;
	}
}

uint8_t gehl_prediction(uint32_t pc){
	S = M/2;
	for(int i = 0; i < M; i++){
		gIndex = index(pc, gHistoryArray[i], i);
		gBht = gehl[i];
		if(gBht.find(gIndex) != gBht.end()){
			S += gBht[gIndex]->value;
		} 
		else{
			gBht.insert(make_pair(gIndex, new SignedCounter(counterBits)));
			S += gBht[gIndex]->value;
		}
	}
	
	prediction = (S >= 0) ? TAKEN : NOTTAKEN;
	return prediction;
}

uint32_t index(uint32_t pc, uint32_t gh, int table_id){
	
	int index_len = 11;
	int length = pow(2.0, table_id + 1);
	uint32_t index = 0;
	uint32_t val_temp;
	uint32_t mask_pc = 1;
	mask_pc = (mask_pc << index_len) - 1;
	uint32_t mask_gh = 1;
	mask_gh = (mask_gh << length) - 1;

	for (int i = 0; i <= 32 / index_len;i = i + 1) {
		index = index ^ (pc & mask_pc);
		pc = pc >> index_len;
	}

	val_temp = gh & mask_gh;

	for (int i = 0; i <= 32 / index_len; i = i + 1) {
		index = index ^ (val_temp & mask_pc);
		val_temp = val_temp >> index_len;
	}

	return index;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

  switch (bpType) {
    case STATIC:
		gBht[pc] = new SignedCounter(1, outcome);
		break;
    case TWOBIT:
		if(outcome){
			if(gBht[pc]->value != ST)
				gBht[pc]->increment();
		}
		else{
			if(gBht[pc]->value != SN)
				gBht[pc]->decrement();
		}
		break;
    case GSHARE:
		if(outcome){
			gBht[gIndex]->increment();
		}
		else{
			gBht[gIndex]->decrement();
		}
		break;
    case TOURNAMENT:
		if(outcome){
			if(gBht[gIndex]->value != ST)
				gBht[gIndex]->increment();
			if(localPredictionMap[lIndex] != ST)
				localPredictionMap[lIndex]++;
		}
		else{
			if(gBht[gIndex] != SN)
				gBht[gIndex]->decrement();
			if(localPredictionMap[lIndex] != SN)
				localPredictionMap[lIndex]--;
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
		break;
    case CUSTOM:
		if(prediction != outcome || S < theta){
			for(int i = 0; i < M; i++){
				gBht = gehl[i];
				gIndex = index(pc, gHistoryArray[i],i);
				if(outcome){
					gBht[gIndex]->increment();
				}
				else{
					gBht[gIndex]->decrement();
				}
			}
		}
    default:
      break;
  }
  gHistory = ((gHistory << 1) | outcome) & gMask;
  lBht[pcIndex].second = ((lBht[pcIndex].second << 1) | outcome) & lMask;

  // CUSTOM
  for(int i = 0 ; i< M ; i++){
	  gHistoryArray[i] = ((gHistoryArray[i] << 1) | outcome) & gMaskArray[i];
  }
}


