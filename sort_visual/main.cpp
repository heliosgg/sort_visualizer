#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#define ARR_SIZE 350
#define SORT_DELAY 1
#define START_BTN sf::Keyboard::Space

#define HEIGHT_SCALE 2
#define LINE_WIDTH 3
#define LINE_GAP 2

#define ALGO_SORT       0
#define ALGO_BUBBLE     1
#define ALGO_PYRAMID    2
#define ALGO_DPYRAMID   3 // TODO: fix this shit. works bad
#define ALGO_COCKTAIL   4
#define ALGO_QUICKSORT  5

#define CURRENT_ALGO ALGO_SORT

const const char* AlgoNames[] = {
   "std::sort",
   "Bubble sort",
   "Pyramid sort",
   "Double-Pyramid sort",
   "Cocktail shaker sort",
   "Quick sort"
};

using namespace std;

// Pointers to current compared elements (Red elements)
int * pCurA;
int * pCurB;

void DrawArray(int* arr,
   int count,
   float width,
   float gap,
   float heightScale,
   float x,
   float y,
   sf::RenderWindow& window,
   bool(*compare)(int&, int&));

struct sortCrutch_t
{
   int* arr;
   int count;
};

void SortFunc(sortCrutch_t* obj);

void BubbleSort         (int* start, int* end, bool(*compare)(int&, int&));
void PyramidSort        (int* start, int* end, bool(*compare)(int&, int&));
void DoublePyramidSort  (int* start, int* end, bool(*compare)(int&, int&));
void CocktailSort       (int* start, int* end, bool(*compare)(int&, int&));
void QuickSort          (int* start, int* end, bool(*compare)(int&, int&));

int main(int argc, char** argv)
{
   sf::ContextSettings settings;
   settings.antialiasingLevel = 8;

   sf::RenderWindow window(sf::VideoMode(40 + ARR_SIZE * (LINE_WIDTH + LINE_GAP),
      ARR_SIZE * HEIGHT_SCALE + 40), "sort", sf::Style::Close, settings);

   int numbers[ARR_SIZE];

   srand(time(NULL));

   // Array generation
   for (int i = 0; i < ARR_SIZE; ++i)
   {
      numbers[i] = i + 1;
   }
   random_shuffle(numbers, numbers + ARR_SIZE);

   sortCrutch_t obj;

   obj.arr = numbers;
   obj.count = ARR_SIZE;
   // ****************

   bool thState = true;
   HANDLE hSortTh = NULL;

   sf::Font font;
   font.loadFromFile("C:\\Windows\\Fonts\\consolaz.ttf");

   sf::Text txtAlgoName(AlgoNames[CURRENT_ALGO], font);
   txtAlgoName.setCharacterSize(30);
   txtAlgoName.setFillColor(sf::Color::White);
   txtAlgoName.setPosition(20.f, 20.f);

   while (window.isOpen())
   {
      sf::Event event;
      while (window.pollEvent(event))
      {
         if (event.type == sf::Event::Closed)
         {
            window.close();
         }
         else if (event.type == sf::Event::KeyPressed)
         {
            if (event.key.code == START_BTN)
            {
               if (!hSortTh)
               {
                  hSortTh = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SortFunc, &obj, NULL, NULL);
                  continue;
               }

               if (thState)
               {
                  SuspendThread(hSortTh);
               }
               else
               {
                  ResumeThread(hSortTh);
               }

               thState = !thState;
            }
            else if (event.key.code == sf::Keyboard::Escape)
            {
               window.close();
            }
         }
      }

      window.clear();

      DrawArray(numbers, ARR_SIZE, LINE_WIDTH, LINE_GAP,
         HEIGHT_SCALE, 20, ARR_SIZE * HEIGHT_SCALE + 20, window,
         [](int& a, int& b) -> bool { return a < b; });

      window.draw(txtAlgoName);

      window.display();
   }

   return 0;
}

void SortFunc(sortCrutch_t* obj)
{
   auto compare = [](int& a, int& b) -> bool {

      pCurA = &a;
      pCurB = &b;

      Sleep(SORT_DELAY);
      return a < b;
   };

#if CURRENT_ALGO == ALGO_SORT
   std::sort(&(obj->arr[0]), &(obj->arr[obj->count]), compare);
#elif CURRENT_ALGO == ALGO_BUBBLE
   BubbleSort(&(obj->arr[0]), &(obj->arr[obj->count]), compare);
#elif CURRENT_ALGO == ALGO_PYRAMID
   PyramidSort(&(obj->arr[0]), &(obj->arr[obj->count]), compare);
#elif CURRENT_ALGO == ALGO_DPYRAMID
   DoublePyramidSort(&(obj->arr[0]), &(obj->arr[obj->count]), compare);
#elif CURRENT_ALGO == ALGO_COCKTAIL
   CocktailSort(&(obj->arr[0]), &(obj->arr[obj->count]), compare);
#elif CURRENT_ALGO == ALGO_QUICKSORT
   QuickSort(&(obj->arr[0]), &(obj->arr[obj->count]), compare);
#endif

   for (int i = 0, j = obj->count - 1; i < j; ++i, --j)
   {
      pCurA = obj->arr + i;
      pCurB = obj->arr + j;

      Sleep(SORT_DELAY);
   }

   pCurA = nullptr;
   pCurB = nullptr;
}

void DrawArray(int* arr,
   int count,
   float width,
   float gap,
   float heightScale,
   float x,
   float y,
   sf::RenderWindow& window,
   bool(*compare)(int&, int&))
{
   for (unsigned int i = 0; i < count; ++i)
   {
      sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(width, (float)arr[i] * heightScale * -1));

      rect.setPosition(sf::Vector2f(x + i * (width + gap), y));

      if (&arr[i] == pCurA || &arr[i] == pCurB)
      {
         rect.setFillColor(sf::Color::Red);
      }
      else
      {
         if (i != 0 && arr[i] - arr[i - 1] == 1
            || i != count - 1 && arr[i + 1] - arr[i] == 1)
         {
            rect.setFillColor(sf::Color::Green);
         }
         else
         {
            rect.setFillColor(sf::Color::Yellow);
         }
      }

      window.draw(rect);

      rect = sf::RectangleShape(sf::Vector2f(width, 3));
      rect.setPosition(sf::Vector2f(x + i * (width + gap), y - (float)arr[i] * heightScale + 2));
      rect.setFillColor(sf::Color::Black);

      window.draw(rect);
   }
}

void BubbleSort(int* start, int* end, bool(*compare)(int&, int&))
{
   bool flag = true;

   while (flag)
   {
      flag = false;

      for (int* i = start + 1; i < end; ++i)
      {
         if (compare(*i, *(i - 1)))
         {
            flag = true;

            int temp = *i;
            *i = *(i - 1);
            *(i - 1) = temp;
         }
      }
   }
}

void PyramidSort(int* start, int* end, bool(*compare)(int&, int&))
{
   int* piMax = nullptr;

   int* pTempEnd = end - 1;

   while (pTempEnd > start)
   {
      piMax = start;
      for (int* i = start + 1; i <= pTempEnd; ++i)
      {
         if (compare(*piMax, *i))
         {
            piMax = i;
         }
      }

      int iTemp = *pTempEnd;
      *pTempEnd = *piMax;
      *piMax = iTemp;

      pTempEnd--;
   }
}

void DoublePyramidSort(int* start, int* end, bool(*compare)(int&, int&))
{
   int* piMax = nullptr;
   int* piMin = nullptr;

   int* pTempEnd = end - 1;
   int* pTempStart = start;

   while (pTempEnd >= pTempStart)
   {
      piMax = pTempStart;
      piMin = pTempStart;

      for (int* i = pTempStart + 1; i <= pTempEnd; ++i)
      {
         if (compare(*piMax, *i))
         {
            piMax = i;
         }

         if (!compare(*piMin, *i))
         {
            piMin = i;
         }
      }

      int iTemp = *pTempEnd;
      *pTempEnd = *piMax;
      *piMax = iTemp;

      iTemp = *pTempStart;
      *pTempStart = *piMin;
      *piMin = iTemp;

      ++pTempStart;
      --pTempEnd;
   }
}

void CocktailSort(int* start, int* end, bool(*compare)(int&, int&))
{
   bool flag = true;
   bool flag2 = true;

   while (start < end && (flag || flag2))
   {
      flag = false;
      flag2 = false;

      for (int* i = start + 1; i < end; ++i)
      {
         if (compare(*i, *(i - 1)))
         {
            flag = true;

            int temp = *i;
            *i = *(i - 1);
            *(i - 1) = temp;
         }
      }
      end--;

      for (int* i = end - 1; i >= start + 1; --i)
      {
         if (!compare(*(i - 1), *i))
         {
            flag = true;

            int temp = *i;
            *i = *(i - 1);
            *(i - 1) = temp;
         }
      }
      start++;
   }
}


void QuickSort(int* start, int* end, bool(*compare)(int&, int&))
{
   if (((int)end - (int)start) <= 4)
   {
      return;
   }

   int iRight = 0;
   int iLeft = 0;

   int* pivot = start + (((DWORD)end - (DWORD)start) / 8);
   int cpyPivot = *pivot;

   for (int* i = start; i < end; ++i)
   {
      if (i == pivot)
      {
         continue;
      }

      if (compare(*i, *pivot))
      {
         iLeft++;
      }
      else
      {
         iRight++;
      }
   }

   int* pRight = new int[iRight];
   int* pLeft = new int[iLeft];

   int r = 0;
   int l = 0;

   for (int* i = start; i < end; ++i)
   {
      if (i == pivot)
      {
         continue;
      }

      if (compare(*i, *pivot))
      {
         pLeft[l++] = *i;
      }
      else
      {
         pRight[r++] = *i;
      }
   }

   int* j = start;

   for (int i = 0; i < iLeft; ++i)
   {
      *j++ = pLeft[i];
      pCurA = j;

      Sleep(SORT_DELAY);
   }

   *j++ = cpyPivot;
   pCurB = j;

   for (int i = 0; i < iRight; ++i)
   {
      *j++ = pRight[i];
      pCurA = j;

      Sleep(SORT_DELAY);
   }

   delete[] pRight;
   delete[] pLeft;

   QuickSort(start, start + iLeft, compare);
   QuickSort(start + iLeft + 1, end, compare);
}