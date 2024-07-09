#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>    

#define MAX_WORD_LENGTH 100
#define MAX_GROUP_NUM 30
#define Iterations 400
#define MAX_WORDS 4000

//遗传算法个体
typedef struct Individual {
	char chromosome[26];
	float fitness;
}Individual;

//遗传算法种群
typedef struct Population {
	int amount;
	Individual* group;
}Population;

//字典树结点
typedef struct TrieNode {
	struct TrieNode* children[26];
	bool is_end_of_word;
} TrieNode;

Individual* createNewIndividual();
Individual createAncestor();
int DecodeForFitness(Individual individual);
int load_words(const char* filename, char words[][MAX_WORD_LENGTH], int max_words);
float Fitness(Individual individual);
bool SmallRandomProbability();
Individual Mutate(Individual individual);
Individual Mating(Individual father, Individual mother);
Population* createTheFirstGroup(Individual ancestor);
Individual Random_Selection(Population* pop);
Population* createAnewGroup();
int AddNewPopulation(Population* pop, Individual individual);
Individual ChooseBestIndividual(Population* pop);
Population* Generate_next_generation(Population* pop, int x);
void Genetic_Algorithm();
int Decode();
void freeTrie(TrieNode* root);
void freePopulation(Population* pop);
TrieNode* load_Trie();
TrieNode* createNode();
void insertWord(TrieNode* root, const char* word);
bool searchWord(TrieNode* root, const char* word);