#include "GA.h"

#define STA "etaoinshrdlcumwfgypbvkjxqz"
Individual* theBestOne = NULL;
TrieNode* trie = NULL;

//创建新个体，将其适应度设置为-1
Individual* createNewIndividual() {
	Individual* newone = malloc(sizeof(Individual));
	if (newone == NULL) {
		printf("创建新个体时内存溢出！\n");
		return NULL;
	}
	newone->fitness = -1;

	return newone;
}

//通过统计文本中字符出现频率进行初步概率匹配，创建“祖先”作为种群的繁衍起点
Individual createAncestor(){
	Individual newone;
	FILE* fp = fopen("data.txt", "r");
	if (NULL == fp) {
		printf("打开密文失败！\n创建祖先失败！\n");
		exit(EXIT_FAILURE);
	}
	int ascii[128] = { 0 };
	char ch;
	while ((ch = fgetc(fp)) != EOF) {
		//清洗常用标点符号（回车 换行 空格 ！ " ' , - . : ; ?)
		if (ch != 10 && ch != 13 && ch != 32 && ch != 33 && ch != 34 && ch != 39 && ch != 44 && ch != 45 && ch != 46 && ch != 58 && ch != 59 && ch != 63) {
			ascii[ch]++;
		}
	}
	fclose(fp);
	//将统计的字符按出现次数进行排序
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

//为计算适应度进行解密
int DecodeForFitness(Individual individual) {
	char ch;
	int count = 0;
	FILE* fp = fopen("data.txt", "r");
	FILE* fp1 = fopen("decode_data.txt", "w");
	if (NULL == fp || NULL == fp1) {
		printf("文件操作失败！\n");
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
		printf("词数小于3000！\n");
		return 1;//若样本词数小于3000则报错返回
	}
	return 0;
}

//创建字典树结点
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

//插入字典树结点
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

//在字典树中查找单词
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

//加载词典
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

//释放结点空间
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

//加载字典树
TrieNode* load_Trie() {
	TrieNode* Trie = createNode();
	char dictionary[MAX_WORDS][MAX_WORD_LENGTH];
	int dict_size = load_words("dictionary.txt", dictionary, MAX_WORDS);
	if (dict_size <= 0) {
		printf("词典出错！\n");
		freeTrie(Trie);
		return -2;
	}
	for (int i = 0; i < dict_size; i++) {
		insertWord(Trie, dictionary[i]);
	}
	return Trie;
}

//计算适应度
float Fitness(Individual individual) {
	if (DecodeForFitness(individual)) return -2;
	char words_to_check[MAX_WORDS][MAX_WORD_LENGTH];
	int words_count = load_words("decode_data.txt", words_to_check, MAX_WORDS);
	if (words_count <= 0) {
		printf("解密文本出错！\n");
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

//获取概率
bool SmallRandomProbability() {
	// 只在第一次调用时设置种子
	static bool seed_set = false;
	if (!seed_set) {
		srand((unsigned int)time(NULL));
		seed_set = true;
	}

	int random_value = rand() % 100;
	return random_value <= 50;
}

//变异函数 当小概率发生时 基因发生变异
Individual Mutate(Individual individual) {

	// 只在第一次调用时设置种子
	static bool seed_set1 = false;
	if (!seed_set1) {
		srand((unsigned int)time(NULL));
		seed_set1 = true;
	}

	int range = rand() % 2; // 0或1

	int random_integer;

	if (range == 0) {
		// 生成0到7之间的随机整数
		random_integer = rand() % 8;
	}
	else {
		// 生成9到24之间的随机整数
		random_integer = rand() % 16 + 9;
	}

	char temp = individual.chromosome[random_integer];
	individual.chromosome[random_integer] = individual.chromosome[random_integer + 1];
	individual.chromosome[random_integer + 1] = temp;


	return individual;
}

//交配函数，挑选两个个体作为父亲和母亲，孩子的染色体0-8继承父亲，9-25继承母亲
Individual Mating(Individual father, Individual mother) {
	Individual child;
	int i = 0;
	for (i = 0; i < 9; i++) {
		child.chromosome[i] = father.chromosome[i];
	}
	for (i = 9; i < 26; i++) {
		child.chromosome[i] = mother.chromosome[i];
	}
	//小概率下孩子发生变异
	if (SmallRandomProbability()) {
		child = Mutate(child);
	}
	child.fitness = Fitness(child);//计算孩子的适应度
	if (child.fitness == -2) {
		printf("创建孩子时出错！\n");
		exit(EXIT_FAILURE);
	}
	return child;
}

//以“祖先”为蓝本创建第一个种群，种群中包含“祖先”，种群中个体的数量为20
Population* createTheFirstGroup(Individual ancestor) {
	Population* FG = createAnewGroup();
	FG->group[0] = ancestor;
	FG->amount++;
	for (int i = 1; i < MAX_GROUP_NUM; i++) {
		Individual newone = ancestor;

		newone = Mutate(newone);
		newone.fitness = Fitness(newone);
		if (newone.fitness == -2) {
			printf("创建新个体时出错！\n");
			exit(EXIT_FAILURE);
		}
		FG->group[i] = newone;
		FG->amount++;
	}
	Individual theGoodOne;
	theGoodOne = ChooseBestIndividual(FG);
	printf("第0代最佳个体的适应度为%f\n", theGoodOne.fitness);
	if (theGoodOne.fitness > theBestOne->fitness) {
		*theBestOne = theGoodOne;
	}
	return FG;
}

//随机选择函数，运用轮盘赌的策略从种群中选择个体进行繁殖，其中个体的选择概率与其适应度成正比。
Individual Random_Selection(Population* pop) {
	float sum = 0, selectNum = 0;
	float* popFN = malloc(sizeof(float) * pop->amount);
	if (popFN == NULL) {
		printf("分配内存失败！\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < pop->amount; i++) {
		popFN[i] = pop->group[i].fitness;
		sum += popFN[i];
	}
	// 只在第一次调用时设置种子
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

//创建一个新的种群
Population* createAnewGroup() {
	Population* NG = malloc(sizeof(Population));
	if (NG == NULL) {
		printf("创建种群时内存溢出！\n");
		return NULL;
	}
	NG->amount = 0;
	NG->group = malloc(sizeof(Individual) * MAX_GROUP_NUM);
	if (NG->group == NULL) {
		printf("创建种群时内存溢出！\n");
		return NULL;
	}
	return NG;
}

//释放种群空间
void freePopulation(Population* pop) {
	if (pop) {
		if (pop->group) {
			free(pop->group);
		}
		free(pop);
	}
}

//添加新的个体到种群中
int AddNewPopulation(Population* pop, Individual individual) {
	if (pop->amount < MAX_GROUP_NUM) {
		pop->group[pop->amount] = individual;
		pop->amount++;
		return 0;
	}
	else{
		printf("超出种群上限！\n");
		return 1;
	}
}

//返回当前种群中的最佳个体
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

//繁衍下一代种群
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
			printf("创建第%d代种群失败！\n", x);
			freePopulation(NewPop);
			return NULL;
		};
	}
	Individual theGoodOne;
	theGoodOne = ChooseBestIndividual(NewPop);
	printf("第%d代最佳个体的适应度为：%f\n", Iterations - x, theGoodOne.fitness);
	if (theGoodOne.fitness > theBestOne->fitness) {
		*theBestOne = theGoodOne;
	}
	freePopulation(pop);
	return NewPop;
}

//解密算法
int Decode() {
	char ch;
	int count = 0;
	FILE* fp = fopen("code.txt", "r");
	FILE* fp1 = fopen("decode.txt", "w");
	if (NULL == fp || NULL == fp1) {
		printf("文件操作失败！\n");
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

//遗传算法主方法
void Genetic_Algorithm() {
	theBestOne = createNewIndividual();
	trie = load_Trie();
	Individual ancestor = createAncestor();
	Population* Pop = createTheFirstGroup(ancestor);
	int x = Iterations;

	while (x--) {
		if (Pop == NULL) {
			printf("繁衍进程中断！\n");
			printf("全局最佳个体适应度为：%f\n", theBestOne->fitness);
			return;
		}
		Pop = Generate_next_generation(Pop, x);
	}

	printf("全局最佳个体适应度为：%f\n", theBestOne->fitness);
	freePopulation(Pop);
	freeTrie(trie);
	return;
}