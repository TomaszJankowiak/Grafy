#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <sstream>

using namespace std;

const int V=50; //! liczba wierzcholkow w grafie; 10, 25, 50, 100, 200, 500
int **G; // macierz sasiedztwa
const int INF = 2147483647; // najwieksza liczba calkowita
fstream plik3;


void zeruj(int *G[V]){ // zeruje macierz sasiedztwa
    for(int i=0;i<V;i++)
        for(int j=0;j<V;j++)
            G[i][j]=0; // 0 oznacza brak krawędzi
}

void macierz_sasiedztwa(){
    G = new int *[V]; // przydzielenie pamieci na macierz sasiedztwa
    for(int a=0; a<V; a++)
        G[a] = new int [V];
    zeruj(G); // 0 oznacza brak krawedzi
}

void usun(){ // usuwa macierz sasiedztwa
    for(int a=0; a<V; a++)
        delete[] G[a];
    delete[] G;
}

void graf(){
    fstream plik;
    int q,f,x,y,z,m; // q- zmienna pomocnicza do zliczania liczby krawedzi, f- wezel startowy, x,y- krawedz, z- jej waga
    float d; // d- gestosc grafu
    d=0.5; //! d- gestosc grafu; 0.25, 0.5, 0.75, 1
    m=d*(V*(V-1)); // liczba krawedzi
    plik.open("stografow.txt",ios::out | ios::app);
    for(int i=0; i<100; i++){
        f=rand()%(V); // losowanie wezla startowego
        plik << m << " " << V << " " << f << endl; // zapisanie liczby krawedzi i wezlow oraz numeru wezla startowego
        if(d!=1){
            for(x=0; x<V-1; x++){ // zapewnia polaczenie miedzy kolejnymi wezlami, aby nie bylo izolowanych wierzcholkow
                y=x+1;
                z=rand()%500+1; // losowanie wagi od 1 do 500
                plik << x << " " << y << " " << z << endl;
            }
            z=rand()%500+1;
            plik << x << " " << 0 << " " << z << endl; // jw- trzeba doprowadzic krawedz do zerowego wezla jeszcze
            q=m-(x+1); // tyle krawedzi jeszcze trzeba dopisac
            macierz_sasiedztwa(); // tutaj jest to tylko macierz pomocnicza
            for(int j=0; j<q; j++){
                x=rand()%V;
                y=rand()%V;
                while(x==y || x==(y-1) || ((V-1)==x && 0==y) || 0!=G[x][y]){ // by nie bylo powtarzajacych sie krawedzi lub petli do tego samego wezla
                    x=rand()%V;
                    y=rand()%V;
                }
                z=rand()%500+1;
                G[x][y]=z;
                plik << x << " " << y << " " << z << endl;
            }
            usun(); // niestety
        }
        else if(1==d){ // usprawnia dzialanie, nie trafiajac caly czas na powtorzenie krawedzi
            for(x=0; x<V; x++){
                for(y=0; y<V; y++){
                    if(x!=y){ // petle sa bez sensu
                        z=rand()%500+1;
                        plik << x << " " << y << " " << z << endl;
                    }
                }
            }
        }
    }
    plik.close();
}

void wypisz(int **G) { // wypisuje macierz sasiedztwa
    cout << endl;
    for(int i=0;i<V;i++){
        for(int j=0;j<V;j++)
            cout<<G[i][j]<<" ";
        cout << endl;
    }
}

struct TNode{
  int wierzch;            // numer wierzchołka
  int waga;          // waga krawędzi
  struct TNode * nast; // następny element listy
};

struct dane{ // do algorytmu Dijkstry na macierzy sasiedztwa
	int koszt;
	int poprz; // poprzednik
	bool odwiedzony; // informacja czy zostal juz odwiedzony
};

int znajdz_min(int v, dane* tab){ // znajduje dla nastepnej iteracji element, ktory ma najkrotszy dystans; zwraca numer wierzcholka, ktory nalezy wybrac
	int min = -1;
	int minkoszt = INF;
	for(int i=0;i<v;i++){
		if(!tab[i].odwiedzony && tab[i].koszt < minkoszt){
			min = i;
			minkoszt = tab[i].koszt;
		}
	}
	return min;
}

dane* Dijkstra(int** macierz, int v, int f){ // glowna funkcja algorytmu; f- wezel startowy, v- l.wezlow
	dane* tab = new dane[v];
	for (int i=0;i<v;i++){
		tab[i].koszt = (i==f) ? 0 : INF;
		tab[i].odwiedzony = false;
		tab[i].poprz = -1;
	}
	int u = znajdz_min(v,tab);
	while (u!=-1){
		tab[u].odwiedzony = true;
		for (int i=0;i<v;i++) {
			if (macierz[u][i]>0 && tab[u].koszt + macierz[u][i] < tab[i].koszt){
				tab[i].koszt = tab[u].koszt + macierz[u][i];
				tab[i].poprz = u;
			}
		}
		u = znajdz_min(v,tab);
	}
	return tab;
}

void zapisz_dane(int i, dane d, int f){
	plik3 << i << ":\t\t";
	plik3 << d.koszt << ";\t\t";
	if(d.koszt<100) plik3 << "\t";
    if (d.poprz == -1)
        plik3 << f;
    else plik3 << f << " ... " << d.poprz << " " << i;
	plik3 << endl;
}


int main()
{
    srand(time(NULL));
    clock_t start, koniec; // potrzebne do zmierzenia czasu dzialania algorytmu
    float roznica=0, czas=0, t=0; // j.w.
    int m=5,v=V,f,x,y,z,j,i=0,l=0,o=0,u,*d,*c,*S,sptr; // m- liczba krawedzi, v-l.wezlow, f- wierzcholek startowy, x,y- krawedz, z- jej waga, j,i- parametry petli for, l,o,u- pomocnicze
    graf(); // tworzenie pliku stografow.txt
    bool *QS;           // Zbiory Q i S
    TNode **graf;     // Tablica list sąsiedztwa
    TNode *pw, *rw;
    string linia;
    fstream plik, plik2;
    plik.open("stografow.txt", ios::in);
    if(plik.good()==false) cerr<<"Blad. Nie mozna otworzyc pliku!";
    plik2.open("rozwiazanie.txt",ios::out | ios::app);

    // algorytm Dijkstry dla grafow na macierzy sasiedztwa
    plik3.open("rozwiazanie.txt",ios::out | ios::app);
    for(int a=0; a<100; a++){
        macierz_sasiedztwa();
        while(getline(plik, linia)){
            stringstream stream(linia);
            while(1){
                stream >> x >> y >> z; // odczytujemy 3 liczby z linijki
                if(!stream) break;
                if(o!=0){
                    G[x][y]=z;
                    l++;
                }
                else if(0==o){
                    m=x; v=y; f=z; // odczytujemy liczbe krawedzi i wierzcholkow oraz wezel startowy
                }
                o++;
            }
            if(m==l) {o=0; l=0; break;}
        }
        start = clock();
        dane* tab = Dijkstra(G,v,f);
        koniec = clock();
        roznica = koniec - start; // roznica- czas dzialania algorytmu dla 1 grafu
        t=t+roznica; // t- czas dzialania algorytmu dla 100 grafow
        plik3 << "wezel startowy: " << f << endl;
        plik3 << "Wezel;  koszt drogi;  ciag wierzcholkow" << endl;
        for (i=0;i<v;i++)
            zapisz_dane(i,tab[i],f);
        plik3 << endl;
        usun();
    }

//! trzeba sobie odkomentowac i zakomentowac tamten wyzej
 /*    // algorytm Dijkstry dla grafow na listach sasiedztwa
    d = new int [v];  // Tablica kosztów dojścia od wierzchołka v do wierzchołka i-tego wzdłuż najkrótszej ścieżki
    c = new int [v];  // Tablica poprzedników. Dla i-tego wierzchołka grafu c[i] zawiera numer wierzchołka poprzedzającego na najkrótszej ścieżce
    QS = new bool [v];  // Zbiory Q i S; S- zbiór wezlow o policzonych już najkrótszych ścieżkach od wezla f, Q- zbiór wezlow, dla których najkrótsze ścieżki nie zostały jeszcze policzone
    graf = new TNode *[v]; // Tablica list sąsiedztwa
    S = new int [v];          // Stos
    sptr = 0;                 // Wskaźnik stosu

    for(int a=0; a<100; a++){
      struct TNode *L[V],*p;
      for(i=0; i<V; i++) L[i] = NULL;
      while(getline(plik, linia)){
        stringstream stream(linia);
        while(1){
            stream >> x >> y >> z; // odczytujemy 3 liczby z linijki
            if(!stream) break;
            if(o!=0){   // stworzenie i wypelnienie listy sasiedztwa na podstawie pliku
                p = new TNode; // Tworzymy element listy sąsiedztwa
                p->nast = L[x]; p->wierzch = y; p->waga = z; L[x] = p; // dolaczenie elementu do listy
                l++;
            }
            else if(0==o){
                m=x; v=y; f=z; // odczytujemy liczbe krawedzi i wierzcholkow oraz wezel startowy
            }
            o++;
        }
        if(m==l) {o=0; l=0; break;}
      }

    start = clock(); // wlasciwie tutaj zaczyna sie algorytm, ktory juz obejmuje wypelnienie ponizszych tablic

    for(i=0; i<v; i++){ // Inicjujemy tablice dynamiczne
        d[i] = INF;
        c[i] = -1;
        QS[i] = false;
        graf[i] = NULL;
    }

    d[f] = 0;   // Koszt dojścia f jest zerowy
    // Wyznaczamy ścieżki
    for(i=0; i<v; i++){
        // Szukamy wierzchołka w Q o najmniejszym koszcie d
        for(j=0; QS[j]; j++);
        for(u=j++; j<v; j++)
            if(!QS[j] && (d[j] < d[u])) u=j;
        // Znaleziony wierzchołek przenosimy do S
        QS[u] = true;
        // Modyfikujemy odpowiednio wszystkich sąsiadów u, którzy są w Q
        for(p = L[u]; p; p = p->nast)
            if(!QS[p->wierzch] && (d[p->wierzch] > d[u] + p->waga)){
                d[p->wierzch] = d[u] + p->waga;
                c[p->wierzch] = u;
              }
    }
    koniec = clock(); // koniec działania algorytmu Dijkstry- zapisanie do pliku to juz nie algorytm
    roznica = koniec - start; // roznica - czas dzialania algorytmu dla 1 grafu
    t=t+roznica; // t - czas dzialania algorytmu dla 100 grafow

    // zapisanie wynikow do pliku
    plik2 << "wezel startowy: " << f << endl;
    for(i=0; i<v; i++){
        plik2 << i << ":  ";
        if(i<10) plik2 << "  "; // estetyka w pliku rozwiazanie.txt
        plik2 << "koszt drogi: ";// << d[i] << ";";
        if(d[i]<100) plik2 << "  "; // j.w.
        if(d[i]<10) plik2 << "  "; //j.w.
        plik2 << d[i] << ";   ";
        plik2 << "ciag wierzcholkow: ";
        // Ścieżkę przechodzimy od końca ku początkowi, zapisując na stosie kolejne wierzchołki
        for(j=i; j>-1; j = c[j]) S[sptr++] = j;
        while(sptr) plik2 << " " << S[--sptr];
        plik2 << endl;
    }
    plik2 << endl;
    }

    // Usuwamy tablice dynamiczne
    delete[ ] d;
    delete[ ] c;
    delete[ ] QS;
    delete[ ] S;
    for(i=0; i<v; i++){
        pw = graf [i];
        while(pw){
            rw = pw;
            pw = pw->nast;
            delete rw;
        }
    }
    delete[ ] graf;
*/

    plik.close();
    plik2.close();
    plik3.close();

    czas = ((float)t)/CLOCKS_PER_SEC; // przeliczenie ilosci taktow zegara na sekundy
    cout << "Czas dzialania algorytmu wyniosl: " << czas << " sekund. " << endl;

    return 0;
}
