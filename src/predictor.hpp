//========================================================//
//  predictor.h                                           //
//  Header file for the Branch Predictor                  //
//                                                        //
//  Includes function prototypes and global predictor     //
//  variables and defines                                 //
//========================================================//

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <stdint.h>
#include <stdlib.h>
#include <set>
#include <iostream>
#include <map>

using namespace std;

//
// Student Information
//
extern const char *studentName;
extern const char *studentID;
extern const char *email;

#define M 4
#define N 2048
#define theta 4

//------------------------------------//
//      Global Predictor Defines      //
//------------------------------------//
#define NOTTAKEN  0
#define TAKEN     1

// The Different Predictor Types
#define STATIC      0
#define GSHARE      1
#define TOURNAMENT  2
#define CUSTOM      3
#define TWOBIT      4
extern const char *bpName[];

// Definitions for 2-bit counters
#define SN  0			// predict NT, strong not taken
#define WN  1			// predict NT, weak not taken
#define WT  2			// predict T, weak taken
#define ST  3			// predict T, strong taken

// Definitions for tournament selector
#define SL  0			// predict NT, strong not taken
#define WL  1			// predict NT, weak not taken
#define WG  2			// predict T, weak taken
#define SG  3			// predict T, strong taken

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//
extern int ghistoryBits; // Number of bits used for Global History
extern int lhistoryBits; // Number of bits used for Local History
extern int pcIndexBits;  // Number of bits used for PC index
extern int bpType;       // Branch Prediction Type
extern int verbose;

//------------------------------------//
//    Predictor Function Prototypes   //
//------------------------------------//

// Initialize the predictor
//
void init_predictor();

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc);

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome);

uint8_t gshare_prediction(uint32_t pc);

uint8_t local_prediction(uint32_t pc);

uint8_t gehl_prediction(uint32_t pc);

uint32_t index(uint32_t pc, uint32_t gh, int table_id);
#endif
