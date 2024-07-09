#include "GA.h"

#define STA "etaoinshrdlcumwfgypbvkjxqz"
Individual* theBestOne = NULL;
TrieNode* trie = NULL;

//�����¸��壬������Ӧ������Ϊ-1
Individual* createNewIndividual() {
	Individual* newone = malloc(sizeof(Individual));
	if (newone == NULL) {
		printf("�����¸���ʱ�ڴ������\n");
		return NULL;
	}
	newone->fitness = -1;

	return newone;
}

//ͨ��ͳ���ı����ַ�����Ƶ�ʽ��г�������ƥ�䣬���������ȡ���Ϊ��Ⱥ�ķ������
Individual createAncestor(){
	Individual newone;
	FILE* fp = fopen("data.txt", "r");
	if (NULL == fp) {
		printf("������ʧ�ܣ�\n��������ʧ�ܣ�\n");
		exit(EXIT_FAILURE);
	}
	int ascii[128] = { 0 };
	char ch;
	while ((ch = fgetc(fp)) != EOF) {
		//��ϴ���ñ����ţ��س� ���� �ո� �� " ' , - . : ; ?)
		if (ch != 10 && ch != 13 && ch != 32 && ch != 33 && ch != 34 && ch != 39 && ch != 44 && ch != 45 && ch != 46 && ch != 58 && ch != 59 && ch != 63) {
			ascii[ch]++;
		}
	}
	fclose(fp);
	//��ͳ�Ƶ��ַ������ִ�����������
	for (int i = 0; i < 26; i++) {
		int max = 0;
		int ch;
		for (int j = 0; j < 128; j++) {
			if (ascii[j] > max) {
				max = ascii[j];
				ch = j;
			}
		}
		ascii[ch] = 0;
		newone.chromosome[i] = ch;
	}
	newone.fitness = Fitness(newone);
	
	return newone;
}

//Ϊ������Ӧ�Ƚ��н���
int DecodeForFitness(Individual individual) {
	char ch;
	int count = 0;
	FILE* fp = fopen("data.txt", "r");
	FILE* fp1 = fopen("decode_data.txt", "w");
	if (NULL == fp || NULL == fp1) {
		printf("�ļ�����ʧ�ܣ�\n");
		if (fp) fclose(fp);
		if (fp1) fclose(fp1);
		return 1;
	}
	while ((ch = fgetc(fp)) != EOF) {
		if (ch != 10 && ch != 13 && ch != 32 && ch != 33 && ch != 34 && ch != 39 && ch != 44 && ch != 45 && ch != 46 && ch != 58 && ch != 59 && ch != 63) {
			int i = 0;
			while (i<=25 && individual.chromosome[i] != ch) i++;
			if (i != 26) {
				char decode = STA[i];
				fputc(decode, fp1);
			}
		}
		else if (ch == 32 || ch == 10 || ch == 13) {
			fputc('\n', fp1);
			count++;
		}
	}
	fclose(fp);
	fclose(fp1);
	fp = NULL;
	fp1 = NULL;
	if (count < 3000) {
		printf("����С��3000��\n");
		return 1;//����������С��3000�򱨴���
	}
	return 0;
}

//�����ֵ������
TrieNode* createNode() {
	TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
	if (node) {
		for (int i = 0; i < 26; i++) {
			node->children[i] = NULL;
		}
		node->is_end_of_word = false;
	}
	return node;
}

//�����ֵ������
void insertWord(TrieNode* root, const char* word) {
	TrieNode* node = root;
	for (int i = 0; word[i] != '\0'; i++) {
		int index = word[i] - 'a';
		if (node->children[index] == NULL) {
			node->children[index] = createNode();
		}
		node = node->children[index];
	}
	node->is_end_of_word = true;
}

//���ֵ����в��ҵ���
bool searchWord(TrieNode* root, const char* word) {
	TrieNode* node = root;
	for (int i = 0; word[i] != '\0'; i++) {
		int index = word[i] - 'a';
		if (node->children[index] == NULL) {
			return false;
		}
		node = node->children[index];
	}
	return node->is_end_of_word;
}

//���شʵ�
int load_words(const char* filename, char words[][MAX_WORD_LENGTH], int max_words) {
	FILE* file = fopen(filename, "r");
	if (NULL == file) {
		return 0;
	}
	int count = 0;
	char line[MAX_WORD_LENGTH];
	while (fgets(line, sizeof(line), file) && count < max_words) {
		if (strchr(line, '\n')) {
			line[strcspn(line, "\n")] = 0;
		}
		sscanf(line, "%s", words[count]);
		count++;
	}
	fclose(file);
	return count;
}

//�ͷŽ��ռ�
void freeTrie(TrieNode* root) {
	if (!root) return;
	TrieNode** stack = (TrieNode**)malloc(MAX_WORDS * MAX_WORD_LENGTH * sizeof(TrieNode*));
	int top = 0;
	stack[top++] = root;
	while (top > 0) {
		TrieNode* node = stack[--top];
		for (int i = 0; i < 26; i++) {
			if (node->children[i]) {
				stack[top++] = node->children[i];
			}
		}
		free(node);
	}
	free(stack);
}

//�����ֵ���
TrieNode* load_Trie() {
	TrieNode* Trie = createNode();
	char dictionary[MAX_WORDS][MAX_WORD_LENGTH];
	int dict_size = load_words("dictionary.txt", dictionary, MAX_WORDS);
	if (dict_size <= 0) {
		printf("�ʵ����\n");
		freeTrie(Trie);
		return -2;
	}
	for (int i = 0; i < dict_size; i++) {
		insertWord(Trie, dictionary[i]);
	}
	return Trie;
}

//������Ӧ��
float Fitness(Individual individual) {
	if (DecodeForFitness(individual)) return -2;
	char words_to_check[MAX_WORDS][MAX_WORD_LENGTH];
	int words_count = load_words("decode_data.txt", words_to_check, MAX_WORDS);
	if (words_count <= 0) {
		printf("�����ı�����\n");
		freeTrie(trie);
		return -2;
	}
	int found_words = 0;
	for (int i = 0; i < words_count; i++) {
		if (searchWord(trie, words_to_check[i])) {
			found_words++;
		}
	}
	return (float)found_words / words_count;
}

//��ȡ����
bool SmallRandomProbability() {
	// ֻ�ڵ�һ�ε���ʱ��������
	static bool seed_set = false;
	if (!seed_set) {
		srand((unsigned int)time(NULL));
		seed_set = true;
	}

	int random_value = rand() % 100;
	return random_value <= 50;
}

//���캯�� ��С���ʷ���ʱ ����������
Individual Mutate(Individual individual) {

	// ֻ�ڵ�һ�ε���ʱ��������
	static bool seed_set1 = false;
	if (!seed_set1) {
		srand((unsigned int)time(NULL));
		seed_set1 = true;
	}

	int range = rand() % 2; // 0��1

	int random_integer;

	if (range == 0) {
		// ����0��7֮����������
		random_integer = rand() % 8;
	}
	else {
		// ����9��24֮����������
		random_integer = rand() % 16 + 9;
	}

	char temp = individual.chromosome[random_integer];
	individual.chromosome[random_integer] = individual.chromosome[random_integer + 1];
	individual.chromosome[random_integer + 1] = temp;


	return individual;
}

//���亯������ѡ����������Ϊ���׺�ĸ�ף����ӵ�Ⱦɫ��0-8�̳и��ף�9-25�̳�ĸ��
Individual Mating(Individual father, Individual mother) {
	Individual child;
	int i = 0;
	for (i = 0; i < 9; i++) {
		child.chromosome[i] = father.chromosome[i];
	}
	for (i = 9; i < 26; i++) {
		child.chromosome[i] = mother.chromosome[i];
	}
	//С�����º��ӷ�������
	if (SmallRandomProbability()) {
		child = Mutate(child);
	}
	child.fitness = Fitness(child);//���㺢�ӵ���Ӧ��
	if (child.fitness == -2) {
		printf("��������ʱ����\n");
		exit(EXIT_FAILURE);
	}
	return child;
}

//�ԡ����ȡ�Ϊ����������һ����Ⱥ����Ⱥ�а��������ȡ�����Ⱥ�и��������Ϊ20
Population* createTheFirstGroup(Individual ancestor) {
	Population* FG = createAnewGroup();
	FG->group[0] = ancestor;
	FG->amount++;
	for (int i = 1; i < MAX_GROUP_NUM; i++) {
		Individual newone = ancestor;

		newone = Mutate(newone);
		newone.fitness = Fitness(newone);
		if (newone.fitness == -2) {
			printf("�����¸���ʱ����\n");
			exit(EXIT_FAILURE);
		}
		FG->group[i] = newone;
		FG->amount++;
	}
	Individual theGoodOne;
	theGoodOne = ChooseBestIndividual(FG);
	printf("��0����Ѹ������Ӧ��Ϊ%f\n", theGoodOne.fitness);
	if (theGoodOne.fitness > theBestOne->fitness) {
		*theBestOne = theGoodOne;
	}
	return FG;
}

//���ѡ�������������̶ĵĲ��Դ���Ⱥ��ѡ�������з�ֳ�����и����ѡ�����������Ӧ�ȳ����ȡ�
Individual Random_Selection(Population* pop) {
	float sum = 0, selectNum = 0;
	float* popFN = malloc(sizeof(float) * pop->amount);
	if (popFN == NULL) {
		printf("�����ڴ�ʧ�ܣ�\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < pop->amount; i++) {
		popFN[i] = pop->group[i].fitness;
		sum += popFN[i];
	}
	// ֻ�ڵ�һ�ε���ʱ��������
	static bool seed_set2 = false;
	if (!seed_set2) {
		srand((unsigned int)time(NULL));
		seed_set2 = true;
	}
	selectNum = (rand() % (int)(sum*1000))/1000.0f;
	int i = 0;
	while (selectNum > 0) {
		selectNum -= popFN[i];
		if (selectNum < 0)break;
		i++;
	}
	free(popFN);
	return pop->group[i];
}

//����һ���µ���Ⱥ
Population* createAnewGroup() {
	Population* NG = malloc(sizeof(Population));
	if (NG == NULL) {
		printf("������Ⱥʱ�ڴ������\n");
		return NULL;
	}
	NG->amount = 0;
	NG->group = malloc(sizeof(Individual) * MAX_GROUP_NUM);
	if (NG->group == NULL) {
		printf("������Ⱥʱ�ڴ������\n");
		return NULL;
	}
	return NG;
}

//�ͷ���Ⱥ�ռ�
void freePopulation(Population* pop) {
	if (pop) {
		if (pop->group) {
			free(pop->group);
		}
		free(pop);
	}
}

//����µĸ��嵽��Ⱥ��
int AddNewPopulation(Population* pop, Individual individual) {
	if (pop->amount < MAX_GROUP_NUM) {
		pop->group[pop->amount] = individual;
		pop->amount++;
		return 0;
	}
	else{
		printf("������Ⱥ���ޣ�\n");
		return 1;
	}
}

//���ص�ǰ��Ⱥ�е���Ѹ���
Individual ChooseBestIndividual(Population* pop) {
	float FN = -1;
	int num = -1;
	for (int i = 0; i < MAX_GROUP_NUM; i++) {
		if (pop->group[i].fitness > FN) {
			FN = pop->group[i].fitness;
			num = i;
		}
	}
	return pop->group[num];
}

//������һ����Ⱥ
Population* Generate_next_generation(Population* pop, int x) {
	Population* NewPop = createAnewGroup();
	Individual father, mother, child;
	child = ChooseBestIndividual(pop);
	AddNewPopulation(NewPop, child);
	for (int i = 1; i < MAX_GROUP_NUM; i++) {
		father = Random_Selection(pop);
		mother = Random_Selection(pop);
		child = Mating(father, mother);

		if (AddNewPopulation(NewPop, child)) {
			printf("������%d����Ⱥʧ�ܣ�\n", x);
			freePopulation(NewPop);
			return NULL;
		};
	}
	Individual theGoodOne;
	theGoodOne = ChooseBestIndividual(NewPop);
	printf("��%d����Ѹ������Ӧ��Ϊ��%f\n", Iterations - x, theGoodOne.fitness);
	if (theGoodOne.fitness > theBestOne->fitness) {
		*theBestOne = theGoodOne;
	}
	freePopulation(pop);
	return NewPop;
}

//�����㷨
int Decode() {
	char ch;
	int count = 0;
	FILE* fp = fopen("code.txt", "r");
	FILE* fp1 = fopen("decode.txt", "w");
	if (NULL == fp || NULL == fp1) {
		printf("�ļ�����ʧ�ܣ�\n");
		if (fp) fclose(fp);
		if (fp1) fclose(fp1);
		return 1;
	}
	while ((ch = fgetc(fp)) != EOF) {
		if (ch != 10 && ch != 13 && ch != 32 && ch != 33 && ch != 34 && ch != 39 && ch != 44 && ch != 45 && ch != 46 && ch != 58 && ch != 59 && ch != 63) {
			int i = 0;
			while (i <= 25 && theBestOne->chromosome[i] != ch) i++;
			if (i != 26) {
				char decode = STA[i];
				fputc(decode, fp1);
			}
		}
		else {
			fputc(ch, fp1);
		}
	}
	fclose(fp);
	fclose(fp1);
	return 0;
}

//�Ŵ��㷨������
void Genetic_Algorithm() {
	theBestOne = createNewIndividual();
	trie = load_Trie();
	Individual ancestor = createAncestor();
	Population* Pop = createTheFirstGroup(ancestor);
	int x = Iterations;

	while (x--) {
		if (Pop == NULL) {
			printf("���ܽ����жϣ�\n");
			printf("ȫ����Ѹ�����Ӧ��Ϊ��%f\n", theBestOne->fitness);
			return;
		}
		Pop = Generate_next_generation(Pop, x);
	}

	printf("ȫ����Ѹ�����Ӧ��Ϊ��%f\n", theBestOne->fitness);
	freePopulation(Pop);
	freeTrie(trie);
	return;
}