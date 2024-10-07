#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <stdio.h>
#include <sstream>
#include <string>
#include <string.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <ctime>

#include "LWindow.h"
#include "stat_graph_el.h"
#include "text.h"
#include "cockroach.h"
#include "image.h"
#include "button.h"

SDL_Renderer* gRenderer = NULL;

LWindow gWindow;

struct Record {
    char name[20];
    unsigned int balance;
};

void addRecord(const char* filename, const Record* record) {

    FILE* file = fopen(filename, "ab");
    if (!file) {
        fprintf(stderr, "Ошибка открытия файла.\n");
        return;
    }

    fwrite(record, sizeof(Record), 1, file);

    fclose(file);
}

int search_name(const char* name) {
    // Открываем файл для чтения
    FILE* file = fopen("base.bin", "rb");
    if (!file) {
        fprintf(stderr, "Ошибка открытия файла.\n");
        return -1;
    }

    // Читаем записи из файла и ищем совпадение по имени
    struct Record record;
    int found = 0;
    while (fread(&record, sizeof(struct Record), 1, file)) {
        if (strcmp(record.name, name) == 0) {
            found = 1;
            break;
        }
    }

    // Закрываем файл
    fclose(file);

    if (found)
        return record.balance;
    else
        return -1;
}

// Функция сравнения для сортировки по полю name
bool compareByName(const Record& record1, const Record& record2) {
    return record1.balance > record2.balance;
}

std::vector<Record> readRecords(const char* filename) {
    std::vector<Record> records;

    // Открываем бинарный файл для чтения
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Ошибка открытия файла.\n");
        return records;
    }

    // Читаем записи из файла
    struct Record record;
    while (fread(&record, sizeof(struct Record), 1, file))
        records.push_back(record);

    // Закрываем файл
    fclose(file);

    std::sort(records.begin(), records.end(), compareByName);
    return records;
}

int handleName(std::string inputText) {
    int balance = search_name(inputText.c_str());
    if(balance < 0) {
        Record rec;
        strncpy(rec.name, inputText.c_str(), sizeof(rec.name) - 1);
        rec.name[sizeof(rec.name) - 1] = '\0';  // Установка завершающего нулевого символа
        balance = rec.balance = 100;
        addRecord("base.bin", &rec);
    }
    return balance;
}

void updateRecord(const char* filename, const char* name, unsigned int newBalance) {

    FILE* file = fopen(filename, "r+b");
    if (!file) {
        fprintf(stderr, "Ошибка открытия файла.\n");
        return;
    }

    struct Record record;
    bool recordFound = false;
    int recordIndex = 0;

    while (fread(&record, sizeof(struct Record), 1, file)) {
        if (!strcmp(record.name, name)) {
            recordFound = true;
            break;
        }
        recordIndex++;
    }

    if (recordFound) {
        record.balance = newBalance;
        long offset = sizeof(struct Record) * recordIndex;
        fseek(file, offset, SEEK_SET);
        fwrite(&record, sizeof(struct Record), 1, file);
    }

    fclose(file);
}

void Intro() {
    text Intro("courier.ttf");
    Intro.setFontSize(16*gWindow.getWidth() / 550.0);
    Intro.loadMedia("Игра 'Тараканьи бега'\nГруппа О729Б Касаткин");
    Intro.render( ( gWindow.getWidth() - Intro.getWidth()/1.5 ) , (gWindow.getHeight() - Intro.getHeight()) / 2);
}

bool CheckRates(unsigned int money[4], std::string PlayRate[4][5]) {
    for(int i = 0, S; i < 4; i++) {
        S = 0;
        for(int j = 0; j < 5; j++) {
            if(PlayRate[i][j].size() > 0)
                S += std::stoi(PlayRate[i][j]);
        }
        if (S > money[i])
            return false;
    }
    return true;
}

void GiveRates(unsigned int money[4], std::string PlayRate[4][5], char winner) {
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 5; j++)
            if(PlayRate[i][j].size() > 0) {
                if(winner == j)
                    money[i] += std::stoi(PlayRate[i][j]);
                else
                    money[i] -= std::stoi(PlayRate[i][j]);
            }
}

void updateDataInFile(const char* filename, running_cockroach* arr, int sizee) {
    FILE* file = fopen(filename, "rb+");
    if (!file) {
        fprintf(stderr, "Ошибка открытия файла.\n");
        return;
    }

    struct data Data;

    for (int i = 0; i < sizee; i++) {
        while (fread(&Data, sizeof(struct data), 1, file)) {
            if (!strcmp(Data.alias, arr[i].getData().alias)) {
                Data.ParticipNumb = arr[i].getData().ParticipNumb;
                Data.WinNumb = arr[i].getData().WinNumb;
                fseek(file, -sizeof(struct data), SEEK_CUR);
                fwrite(&Data, sizeof(struct data), 1, file);
                rewind(file);
                break;
            }
        }
    }

    fclose(file);
}

bool SameName(std::string names[], int sizee = 4) {

    std::unordered_set<std::string> uniqueNames;

    for (int i = 0; i < sizee; i++)
        if (!uniqueNames.insert(names[i]).second && names[i] != "") // second чтобы обратиться к результату вставки
            return false;

    // Если все имена уникальны, возвращаем true
    return true;
}

void createTextFileFromRecords(const std::vector<Record>& records, const std::string& fileName) {
    FILE* file = fopen(fileName.c_str(), "w");
    if (!file) {
        std::cerr << "Ошибка открытия файла для записи." << std::endl;
        return;
    }

    fprintf(file, "Records\n");

    for (int i = 0; i < 10 && i < records.size(); i++)
        fprintf(file, "%s\t%u\n", records[i].name, records[i].balance);

    fclose(file);
}

bool init();

void close();

bool init()
{
	bool success = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL не может загрузиться! SDL Ошибка: %s\n", SDL_GetError());
		success = false;
	}
	else
	{   // более красиво вроде
		if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
			printf( "Внимание: Линейная фильтрация текстур не включена!" );
		if(!gWindow.init())
		{
			printf( "Окно не может создаться! SDL Ошибка: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			gRenderer = gWindow.createRenderer();
			if(gRenderer == NULL)
			{
				printf( "Рендер не может создаться! SDL Ошибка: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image не инициализирован! SDL_image Ошибка: %s\n", IMG_GetError() );
					success = false;
				}


				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf не инициализирован! SDL_ttf Ошибка: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

void close()
{
	SDL_DestroyRenderer( gRenderer );
    gRenderer = NULL;
	gWindow.free();

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	if(!init())
		printf("Ошибка инициализации!\n");
    else
    {
        bool quit = false;

        SDL_Event e;

        image img;
        img.loadMedia("cockroachback.png");
        img.setAlpha(100);

        text TextMenu("courier.ttf");
        TextMenu.loadMedia("");

        text TextInputName[4];

        button StartBut;
        button TutorialBut;
        button ChangeNameBut;
        button HighScoreBut;
        button ExitBut;

        button ToMainMenuBut;
        button HelpBut;

        button InvisibleForTextBut[4];

        button ReadyBut[9];

        StartBut.loadMedia("but.png");
        TutorialBut.loadMedia("but.png");
        ChangeNameBut.loadMedia("but.png");
        HighScoreBut.loadMedia("but.png");
        ExitBut.loadMedia("but.png");

        ToMainMenuBut.loadMedia("but.png");
        HelpBut.loadMedia("but.png");

        for(int i = 0; i < 4; i++) {
            InvisibleForTextBut[i].loadMedia("but.png");
            InvisibleForTextBut[i].setAlpha(0);
        }

        for(int i = 0; i < 9; i++) {
            ReadyBut[i].loadMedia("but.png");
            ReadyBut[i].setAlpha(100);
        }

        HelpBut.setAlpha(100);
        StartBut.setAlpha(100);
        TutorialBut.setAlpha(100);
        ChangeNameBut.setAlpha(100);
        HighScoreBut.setAlpha(100);
        ExitBut.setAlpha(100);
        ToMainMenuBut.setAlpha(100);

        unsigned int balance[4];
        int need = -1, needd = -1;
        int butheight, butwidth;
        char tut = 0, game = 0, menu = 0, name = 0, high = 0, readygame = 0, start = 0, endd = 0, sstart = 0, pause = 0;
        bool keyPressed = false;

        running_cockroach ar[5];

        std::string names[4];
        std::string rates[5];
        for (int i = 0; i < 5; i++)
            rates[i] = "Поставить";

        std::string PlayerRates[4][5];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 5; j++)
                PlayerRates[i][j] = "";

        srand(time(NULL));
        int kol = rand() % 4 + 2;

        std::vector<stat_graph_element*> elements;

        elements.push_back(&img); // [0]

        elements.push_back(&StartBut); // [1]
        elements.push_back(&TutorialBut); // [2]
        elements.push_back(&ChangeNameBut); //[3]
        elements.push_back(&HighScoreBut); // [4]
        elements.push_back(&ExitBut); // [5]

        elements.push_back(&HelpBut); // [6]
        elements.push_back(&ToMainMenuBut);  // [7]

        elements.push_back(&TextMenu); // [8]

        while(!quit)
        {
            while(SDL_PollEvent(&e) != 0 || game)
            {
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF ); //очистить экран
                SDL_RenderClear( gRenderer );

                if( e.type == SDL_QUIT) { // выйти по крестику
                    quit = true;
                    break;
                }
                else if (e.type == SDL_KEYDOWN)
                    keyPressed = true;

                gWindow.handleEvent(e); // события окна

                elements[0]->render(0, 0, gWindow.getWidth(), gWindow.getHeight()); // фон

                if(e.type != SDL_KEYDOWN && !keyPressed) // условия чтобы интро убралось по нажатию любой клавиши
                    Intro();

                else if(!name) { /// поля ввода имени

                    for(int i = 0; i < 5; i++)
                        dynamic_cast<button*>(elements[i+1])->disable();

                    int position1 = (gWindow.getHeight() - elements[6]->getHeight()) - 0.1 * elements[6]->getHeight();

                    dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 400.0); // ставим каждый раз потому что адаптивный
                    dynamic_cast<text*>(elements[8])->loadMedia("Ок"); // загружаем каждый раз по той же причине

                    elements[8]->render( ( gWindow.getWidth() - elements[8]->getWidth() ) / 2, position1); // рендерим текст внизу
                    elements[6]->render( ( gWindow.getWidth() - elements[6]->getWidth() ) / 2,  position1,
                                         gWindow.getWidth() / 3, gWindow.getHeight() / 12); // кнопку внизу

                    for(int i = 0; i < 4; i++) { // цикл с рендером полей для ввода
                        char s[32] = "";
                        snprintf(s, sizeof(s), "Имя %d-ого игрока", i+1); // добавляем число в строку

                        TextInputName[i].setFontSize (16 * gWindow.getWidth() / 400.0); // поле ввода имени
                        TextInputName[i].render ( ( gWindow.getWidth() - TextInputName[i].getWidth() ) / 2, (3*i+1) * elements[8]->getHeight() );

                        dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 400.0); // поле "введите имя"
                        dynamic_cast<text*>(elements[8])->loadMedia(s);
                        elements[8]->render ( ( gWindow.getWidth() - elements[8]->getWidth() ) / 2, 3*i * elements[8]->getHeight() );

                        // кнопка - поле для ввода
                        InvisibleForTextBut[i].render ( ( gWindow.getWidth() - InvisibleForTextBut[i].getWidth() ) / 2, (3*i+1) * elements[8]->getHeight(),
                                                       gWindow.getWidth() / 2, gWindow.getHeight() / 12);

                        if(InvisibleForTextBut[i].handleEvent(&e)) {
                            if(need != -1) // need нужен чтобы понимать, в какое поле ввожу данные
                                InvisibleForTextBut[need].setAlpha(0); // и все кнопки делаем невидимыми
                            need = i;
                            InvisibleForTextBut[need].setAlpha(100);
                        }
                    }

                    // если есть хоть одно имя - заканчиваем ввод
                    if(dynamic_cast<button*>(elements[6])->handleEvent(&e) ) {
                        if( (names[0] != "" || names[1] != "" || names[2] != "" || names[3] != "") && SameName(names) ) {
                            for(int i = 0; i < 4; i++)
                                if(names[i] != "") // для каждого имени ищем его баланс в файле
                                    balance[i] = handleName(names[i]);
                            name = 1;
                        }
                        else { // иначе подсвечиваем поля для ввода
                            for(int i = 0; i < 4; i++) {
                                InvisibleForTextBut[i].setAlpha(100);
                                InvisibleForTextBut[i].render ( ( gWindow.getWidth() - InvisibleForTextBut[i].getWidth() ) / 2, (3*i+1) * elements[8]->getHeight(),
                                                               gWindow.getWidth() / 2, gWindow.getHeight() / 12);
                            }
                            for(int i = 0; i < 4; i++)
                                InvisibleForTextBut[i].setAlpha(0);
                        }
                    }

                    if(e.type == SDL_KEYDOWN) //для backspace
                    {
                        if( e.key.keysym.sym == SDLK_BACKSPACE && need != -1 && names[need].length() > 0 )
                            names[need].pop_back();
                    }

                    else if(e.type == SDL_TEXTINPUT) // для ввода имени
                    {
                        if(need != -1 && names[need].size() < 19)
                            names[need] += e.text.text;
                    }

                    if(need != -1)
                        TextInputName[need].loadMedia(names[need].c_str());
                }

                else if(dynamic_cast<button*>(elements[1])->handleEvent(&e) || readygame) { /// подготовка к игре - ставки v

                    for(int i = 0; i < 5; i++)
                        dynamic_cast<button*>(elements[i+1])->disable();

                    char n[5] = ""; // цифры в строки
                    char p[5] = ""; // цифры в строки

                    for(int i = 0, j = 0; i < 4; i++) // проходимся по массиву имен
                        if(names[i] != "") { // если имя было введено, то рендерим связанные с ним объекты
                            snprintf(n, sizeof(n), "%d", balance[i]); // число в строку
                            double pos = ( (gWindow.getWidth() / 4) * j++) + gWindow.getWidth() / 20;

                            dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 500.0);
                            dynamic_cast<text*>(elements[8])->loadMedia(names[i] + "\n\n\nБаланс:" + n + "\n\n");
                            elements[8]->render ( pos, gWindow.getHeight() / 18 );

                            ReadyBut[i].render(pos, gWindow.getHeight() / 9 ,gWindow.getWidth() / 6, gWindow.getHeight() / 12);

                            dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 500.0);
                            dynamic_cast<text*>(elements[8])->loadMedia("Выбрать");
                            elements[8]->render(pos, gWindow.getHeight() / 8);
                        }

                    for(int i = 0; i < kol; i++) { // цикл для тараканов, их инфы
                        ar[i].readData("Data.bin", i); // данные из файла
                        snprintf(n, sizeof(n), "%d", ar[i].getData().ParticipNumb);
                        snprintf(p, sizeof(p), "%d", ar[i].getData().WinNumb);
                        double pos = ((gWindow.getWidth() / 5) * i) + gWindow.getWidth() / 80;

                        dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 700.0);
                        dynamic_cast<text*>(elements[8])->loadMedia(std::string(ar[i].getData().alias) + "\n\nЗабегов:" + n + "\n\nПобед:" + p);
                        elements[8]->render (pos, gWindow.getHeight() / 2);

                        ReadyBut[i+4].render(pos, gWindow.getHeight() - 3*ReadyBut[i+4].getHeight(), gWindow.getWidth() / 6, gWindow.getHeight() / 12);

                        dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 600.0); // поле для ввода ставки
                        dynamic_cast<text*>(elements[8])->loadMedia(rates[i]);
                        elements[8]->render(pos, gWindow.getHeight() - 2.9*ReadyBut[i+4].getHeight());
                    }

                    for(int i = 0; i < 4; i++) // цикл по игрокам
                        if(ReadyBut[i].handleEvent(&e)) { // если click
                            for(int j = 0; j < 4; j++)
                                if(j != i)
                                    ReadyBut[j].setAlpha(100); // все остальные делаем обычными
                            ReadyBut[i].setAlpha(255); // ее особенной
                            need = i; // запоминаем игрока
                        }

                    for(int i = 0; i < kol; i++) // цикл по ставкам
                        if(ReadyBut[i+4].handleEvent(&e)) { // если click
                            for(int j = 0; j < 5; j++)
                                if(j != i)
                                    ReadyBut[j+4].setAlpha(100);  // все остальные делаем обычными
                            ReadyBut[i+4].setAlpha(255); // ее особенной
                            needd = i; // запоминаем таракана
                        }

                    if(need != -1 && needd != -1) { // если есть и ставка и таракан
                        for(int i = 0; i < kol; i++) // идем по ставкам
                            rates[i] = PlayerRates[need][i]; // присваиваем элементу массива ставок то, что хранится в матрице игрок - ставка
                        rates[needd] = PlayerRates[need][needd]; // а здесь писваиваем с особоенным индексом, так как отличное от других
                    }

                    if( e.type == SDL_KEYDOWN ) // backspace ставки
                    { // контроль backspace
                        if( e.key.keysym.sym == SDLK_BACKSPACE && need != -1 && needd != -1 && PlayerRates[need][needd].length() > 0 )
                            PlayerRates[need][needd].pop_back();
                    }
                    else if( e.type == SDL_TEXTINPUT)  // ввод ставки
                    { // контроль ввода
                        if(!strcmp(e.text.text, "0") || !strcmp(e.text.text, "1") || !strcmp(e.text.text, "2") || !strcmp(e.text.text, "3") ||
                           !strcmp(e.text.text, "4") || !strcmp(e.text.text, "5") || !strcmp(e.text.text, "6") || !strcmp(e.text.text, "7") ||
                           !strcmp(e.text.text, "8") || !strcmp(e.text.text, "9") )
                            if( need != -1 && needd != -1 && PlayerRates[need][needd].size() < 6 )
                                PlayerRates[need][needd] += e.text.text; // добавляем число в строку, хранящейся в матрице строк
                    }

                    if( need != -1 && needd != -1) // если есть игрок и ставка
                        rates[needd] = PlayerRates[need][needd]; // писваиваем тому, что будем рендерить это значение(обновляем)

                    dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 470.0); // текст в главное меню
                    dynamic_cast<text*>(elements[8])->loadMedia("Назад в меню");
                    elements[8]->render( elements[7]->getWidth() / 100, gWindow.getHeight() - elements[7]->getHeight() - 0.3*elements[7]->getHeight() );

                    // кнопка в главное меню
                    elements[7]->render( elements[7]->getWidth() / 100, gWindow.getHeight() - elements[7]->getHeight() - 0.5*elements[7]->getHeight(), gWindow.getWidth() / 4, gWindow.getHeight() / 12 );

                    dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 470.0); // текст начать игру
                    dynamic_cast<text*>(elements[8])->loadMedia("Начать игру");
                    elements[8]->render( ( gWindow.getWidth() - elements[6]->getWidth() ) / 2, gWindow.getHeight() - 1.4*elements[6]->getHeight());

                    // кнопка начать игру
                    elements[6]->render( (gWindow.getWidth() - elements[6]->getWidth()) / 2, gWindow.getHeight() - 1.5*elements[6]->getHeight(), gWindow.getWidth() / 4, gWindow.getHeight() / 12 );

                    readygame = 1;

                    if( dynamic_cast<button*>(elements[7])->handleEvent(&e) ) // выход в меню если click
                        readygame = 0;

                    if( dynamic_cast<button*>(elements[6])->handleEvent(&e) && CheckRates(balance, PlayerRates)) { // начало игры если click
                        readygame = 0;
                        game = 1;
                        start = 0;
                        endd = 0;
                    }
                }

                else if(game || sstart) { /// игра V

                    for(int i = 0; i < 5; i++)
                        dynamic_cast<button*>(elements[i+1])->disable();

                    std::string cockroach[5] = { // массив цветов чтобы не громоздить циклыыы
                        {"red"},
                        {"blue"},
                        {"green"},
                        {"yellow"},
                        {"pink"}
                    };

                    image line; // сама "карта"
                    line.loadMedia("line.png");
                    line.render(0, 0, gWindow.getWidth(), gWindow.getHeight() / 1.8);

                    for(int i = 0; i < kol && !start; i++) { // рендерим тараканов на своих начальных позициях
                        double pos = i ? (gWindow.getHeight() / 1.8) / (5./i) : i;
                        ar[i].loadMedia("run-cockroach-" + cockroach[i] + ".png");
                        ar[i].setPos(0, pos);
                    }

                    start = 1;

                    for(int i = 0, j = 0; i < 4; i++) // цикл рендерим имена, а ниже их ставки
                        if(names[i] != "") {
                            double pos = ( (gWindow.getWidth() / 4) * j) + gWindow.getWidth() / 15;
                            dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 500.0);
                            dynamic_cast<text*>(elements[8])->loadMedia(names[i]);
                            elements[8]->render ( pos, gWindow.getHeight() / 1.8 );

                            for(int k = 0; k < kol; k++) { // для каждого имени - рендерим все ставки
                                pos = ( (gWindow.getWidth() / 4) * j) + gWindow.getWidth() / 15;
                                dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 650.0);
                                // чтобы не пустая строка, а 0 и при этом красиво в readygame v
                                dynamic_cast<text*>(elements[8])->loadMedia(std::string(ar[k].getData().alias) + ":" + ( PlayerRates[i][k] == "" ? "0" : PlayerRates[i][k]));
                                elements[8]->render( pos - 0.25* elements[8]->getWidth(), (gWindow.getHeight() / 1.5) + (k *  elements[8]->getHeight())  );
                            }
                            j++;
                        }

                    for(int i = 0; i < kol && !pause; i++) { // вся игра
                        int ran = rand() % 10 - 3;  // -3 потому что иначе не дождаться

                        ar[i].setVel(ran, 0); // ставим скорость
                        if(!sstart) {
                            ar[i].move(); // двигаем
                            if(ar[i].getPosX() >= gWindow.getWidth() - 1.7*ar[i].getWidth()) // условие победы таракана
                                endd = i+1; // запоминаем индекс победителя, +1 потому что это условие истинности
                        }
                        ar[i].setAlpha(255);
                        ar[i].render(0, 0, gWindow.getWidth() / 8, gWindow.getHeight() / 9); // рендерим
                    }

                    if(e.type == SDL_KEYDOWN && !endd)
                        if(e.key.keysym.sym == SDLK_ESCAPE)
                            pause = !pause;

                    if(pause)
                        for(int i = 0; i < kol; i++) {
                            ar[i].setAlpha(70);
                            ar[i].render(0, 0, gWindow.getWidth() / 8, gWindow.getHeight() / 9); // рендерим
                            elements[7]->render( (gWindow.getWidth() - elements[7]->getWidth() ) / 2, (gWindow.getHeight() - elements[7]->getHeight() ) / 2,
                                       gWindow.getWidth()/1.5, gWindow.getWidth()/5);

                            dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 500.0);
                            dynamic_cast<text*>(elements[8])->loadMedia( "             ПАУЗА \nНажмите ESC, чтобы вернуться \nНажмите мышью, чтобы выйти в меню" );
                            elements[8]->render ( (gWindow.getWidth() - elements[7]->getWidth() ) / 1.9, (gWindow.getHeight() - elements[7]->getHeight() ) / 1.9);
                            if(dynamic_cast<button*>(elements[7])->handleEvent(&e)) {
                                sstart = 0;
                                start = 0;
                                game = 0;
                                pause = 0;
                                kol = rand() % 4 + 2;
                            }
                        }

                    if(endd && game) { // если гонка закончена, раздаем/забираем ставки, обновляем файлы
                        sstart = 1; // чтобы не возвращаться на старт
                        for(int i = 0; i < kol; i++) { // по всем тараканам
                            ar[i].setVel(0, 0);
                            if(endd == i+1)
                                ar[i].GameOver(1); // победителю даем еще +1 к победам
                            else
                                ar[i].GameOver(0); // всем остальным спасибо за участие
                        }
                        GiveRates(balance, PlayerRates, endd-1); // раздаем ставки
                        updateDataInFile("Data.bin", ar, kol); // раздаем забеги и победы

                        game = 0;

                        for(int i = 0; i < 4; i++)
                            if(names[i] != "")
                                updateRecord("base.bin", names[i].c_str(), balance[i]); // обновляем деньги игроков
                    }

                    if(sstart && endd) {
                        elements[7]->render( (gWindow.getWidth() - elements[7]->getWidth() ) / 2, (gWindow.getHeight() - elements[7]->getHeight() ) / 2,
                                       gWindow.getWidth()/2, gWindow.getWidth()/5);

                        dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 500.0);
                        dynamic_cast<text*>(elements[8])->loadMedia( "Победил таракан с кличкой \n" + std::string(ar[endd-1].getData().alias) );
                        dynamic_cast<text*>(elements[8])->render ( (gWindow.getWidth() - elements[7]->getWidth() ) / 1.9, (gWindow.getHeight() - elements[7]->getHeight() ) / 1.9);

                        if(dynamic_cast<button*>(elements[7])->handleEvent(&e)) {
                            sstart = 0;
                            start = 0;
                            kol = rand() % 4 + 2;
                        }
                    }
                }

                else if(dynamic_cast<button*>(elements[2])->handleEvent(&e)|| tut) { /// туториал V

                    for(int i = 0; i < 5; i++)
                        dynamic_cast<button*>(elements[i+1])->disable();

                    dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 550.0);
                    dynamic_cast<text*>(elements[8])->loadMedia("Добро пожаловать в игру 'Тараканьи бега'!\n\n\
Правила игры просты: в каждом забеге участвуют несколько тараканов, которые соревнуются за первое место. Вы можете делать ставки на понравившихся тараканов и, если ваш выбор окажется победителем, получить выигрыш.\n\n\
Каждый таракан бежит по своей дорожке со своей уникальной скоростью. Победителем считается таракан, первым пересекший финишную черту.\n\n\
Готовы отправиться в захватывающий мир тараканьих бегов? Пусть удача всегда будет на вашей стороне!\n\n" );
                    elements[8]->render(0, 0);

                    dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 400.0);
                    dynamic_cast<text*>(elements[8])->loadMedia("Назад в меню");
                    elements[8]->render( (gWindow.getWidth() - elements[8] ->getWidth() ) / 2, gWindow.getHeight() - elements[7]->getHeight() - 0.5*elements[7]->getHeight() );

                    elements[7]->render( (gWindow.getWidth() - elements[7]->getWidth() ) / 2 , gWindow.getHeight() - elements[7]->getHeight() - 0.5*elements[7]->getHeight(),
                                          gWindow.getWidth() / 3, gWindow.getHeight() / 12 );

                    tut = 1;
                    if( dynamic_cast<button*>(elements[7])->handleEvent(&e) )
                        tut = 0;
                }

                else if(dynamic_cast<button*>(elements[3])->handleEvent(&e)) { /// смена имени V
                    name = 0;
                }

                else if(dynamic_cast<button*>(elements[4])->handleEvent(&e) || high) { /// таблица рекордов V

                    for(int i = 0; i < 5; i++)
                        dynamic_cast<button*>(elements[i+1])->disable();

                    std::vector<Record> top = readRecords("base.bin"); // массив записей, читаем весь файл(некруто)

                    dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 450.0);
                    dynamic_cast<text*>(elements[8])->loadMedia("Таблица рекордов");
                    elements[8]->render(( gWindow.getWidth() - elements[8]->getWidth() ) / 2, 0);

                    dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 475.0);
                    dynamic_cast<text*>(elements[8])->loadMedia("Назад в меню");
                    elements[8]->render( elements[7]->getWidth()/100, gWindow.getHeight() - elements[7]->getHeight() - 0.2*elements[7]->getHeight() );

                    elements[7]->render( elements[7]->getWidth()/100, gWindow.getHeight() - elements[7]->getHeight() - 0.5*elements[7]->getHeight(),
                                          gWindow.getWidth() / 4, gWindow.getHeight() / 12 );

                    char num[5] = "";
                    auto end = top.begin() + (top.size() >= 10 ? 10 : top.size()); // итератор на конец
                    int i = 0;

                    // Проходим по диапазону элементов и выполняем необходимые операции
                    for (auto it = top.begin(); it != end; ++it, i++) {
                        snprintf(num, sizeof(num), "%d", it->balance);

                        dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 500.0);
                        dynamic_cast<text*>(elements[8])->loadMedia(it->name);
                        elements[8]->render ( ( gWindow.getWidth() - elements[8]->getWidth() ) / 3.7, 1.4 * (i+1) * elements[8]->getHeight() );

                        dynamic_cast<text*>(elements[8])->loadMedia(num);
                        elements[8]->render ( ( gWindow.getWidth() - elements[8]->getWidth() ) / 1.3, 1.4 * (i+1) * elements[8]->getHeight() );
                    }

                    dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 550.0);
                    dynamic_cast<text*>(elements[8])->loadMedia("Подготовить для печати");
                    elements[8]->render( (gWindow.getWidth() - elements[6]->getWidth()) / 2.1, gWindow.getHeight() - elements[6]->getHeight() - 0.2*elements[6]->getHeight() );

                    elements[6]->render( (gWindow.getWidth() - elements[6]->getWidth()) / 2.1, gWindow.getHeight() - elements[6]->getHeight() - 0.5*elements[6]->getHeight(),
                                          gWindow.getWidth() / 2.5, gWindow.getHeight() / 12);

                    dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 475.0);
                    dynamic_cast<text*>(elements[8])->loadMedia("Удалить данные");
                    elements[8]->render(  gWindow.getWidth() - 1.01*elements[5]->getWidth(), gWindow.getHeight() - elements[5]->getHeight() - 0.2*elements[5]->getHeight() );

                    elements[5]->render( gWindow.getWidth() - 1.01*elements[5]->getWidth() , gWindow.getHeight() - elements[5]->getHeight() - 0.5*elements[5]->getHeight(),
                                          gWindow.getWidth() / 3.5, gWindow.getHeight() / 12);
                    high = 1;

                    if(dynamic_cast<button*>(elements[7])->handleEvent(&e) ) //выход
                        high = 0;

                    if(dynamic_cast<button*>(elements[6])->handleEvent(&e) ) // печать
                    {
                        createTextFileFromRecords(top, "textforprint.txt");
                        dynamic_cast<text*>(elements[8])->loadMedia("Успешно");
                        elements[8]->render(  (gWindow.getWidth() - elements[5]->getWidth()) / 2 , (gWindow.getHeight() - elements[5]->getHeight()) / 2 );
                    }

                    if(dynamic_cast<button*>(elements[5])->handleEvent(&e) ) // удалить
                    {
                        FILE * f = fopen("base.bin", "wb");
                        fclose(f);
                        dynamic_cast<text*>(elements[8])->loadMedia("Успешно");
                        elements[8]->render(  (gWindow.getWidth() - elements[5]->getWidth()) / 2 , (gWindow.getHeight() - elements[5]->getHeight()) / 2 );
                    }
                }

                else if(dynamic_cast<button*>(elements[5])->handleEvent(&e)) { /// кнопка выйти V
                    quit = true;
                    break;
                }

                else { /// главное меню V
                    for(int i = 0; i < 4; i++)
                        if(names[i] != "")
                            balance[i] = handleName(names[i]);

                    std::vector<std::string> wr = {
                        {"Запустить"},
                        {"Справка"},
                        {"Сменить имя"},
                        {"Рекорды"},
                        {"Выход"}
                    };

                    butwidth = gWindow.getWidth() / 4;
                    butheight = gWindow.getHeight() / 16;

                    int gap = elements[1]->getWidth() / 4;
                    int position1 = (gWindow.getHeight() - (5 * elements[1]->getHeight() + 4 * gap)) / 2;

                    for(int i = 0; i < 5; i++) {
                        dynamic_cast<text*>(elements[8])->setFontSize(16 * gWindow.getWidth() / 450.0);
                        dynamic_cast<text*>(elements[8])->loadMedia(wr[i]);
                        elements[8]->render( ( gWindow.getWidth() - elements[8]->getWidth() ) / 2, position1 + i*elements[1]->getHeight() + i*gap);

                        elements[i+1]->render( ( gWindow.getWidth() - elements[i+1]->getWidth() ) / 2,  position1 + i*elements[1]->getHeight() + i*gap, butwidth, butheight);
                    }

                    menu = 1;
                }

                SDL_RenderPresent( gRenderer );
            }
        }
        SDL_StopTextInput();
    }

    close();

	return 0;
}
