#include <iostream>
#include <time.h>
#include <queue>
#include <thread> 

using namespace std;

void printTable(int wt[], int tat[], int burst_time[], int n, int finalTime)
{
	int total_wt = 0, total_tat = 0;
	cout << "Processes"
		 << "\tBurst time"
		 << "\tWaiting time"
		 << "\tTurnaround time\n";

	for (int i = 0; i < n; i++)
	{
		// calculamos el total waiting time y total turnaround time (la suma de todos)
		total_wt += wt[i];
		total_tat += tat[i];
		//imprimimos los datos
		cout << "    " << i + 1 << "\t\t    " << burst_time[i] << "\t\t    " << wt[i] << "\t\t    " << tat[i] << endl;
	}

	cout << "----------------------------------------------------------------"
	     << "\n - Average waiting time = " << (float)total_wt / (float)n
	     << "\n - Average turnaround time = " << (float)total_tat / (float)n
	     << "\n - Average throughput time = " << (float)n / (float)finalTime
		 << "\n________________________________________________________________";
}

// turn around time (tiempos desde que incio hasta que terminó el proceso)
void findTurnAroundTime(int processes[], int n,
						int burst_time[], int wt[], int tat[])
{
	// el turnaround time es la suma del burst time más el waiting time
	for (int i = 0; i < n; i++)
		tat[i] = burst_time[i] + wt[i];
}

// ------------------ ROUND ROBIN -------------------------
// tiempos de espera
void findWaitingTimeRR(int processes[], int n, int burst_time[], int wt[], int quantum, int &t)
{
	// copia de burst_time para almacenar los tiempos restantes
	int rem_burst_time[160];
	for (int i = 0; i < n; i++)
		rem_burst_time[i] = burst_time[i];

	// se recorren los procesos hasta que todos esten listos
	while (true)
	{
		bool done = true;
		// recorremos todos los procesos
		for (int i = 0; i < n; i++)
		{
			// si hay falta tiempo por procesar
			if (rem_burst_time[i] > 0)
			{
				// hay un proceso pendiente
				done = false;

				if (rem_burst_time[i] > quantum)
				{
					// solo se procesa hasta llegar el quantum y se pasas al siguiente procesos
					t += quantum;
					rem_burst_time[i] -= quantum;
				}
				// si es menor al quantum, el procesos será terminado en este ciclo
				else
				{
					t = t + rem_burst_time[i];
					wt[i] = t - burst_time[i]; // el tiemo de espera es el tiempo actual menos el burst time
					rem_burst_time[i] = 0;
				}
			}
		}
		if (done == true)
			break;
	}
	//cout<<"TIEMPO: "<<t<<endl;
}

void RR(int processes[], int n, int burst_time[], int quantum)
{
	int wt[160], tat[160], t = 0;

	// calculamos waiting time y turn around time de cada procesos
	findWaitingTimeRR(processes, n, burst_time, wt, quantum, t);
	findTurnAroundTime(processes, n, burst_time, wt, tat);

	// imprimimos las tabla de procesos con sus datos
	cout << "\n______________________________ RR ______________________________\n";
	printTable(wt, tat, burst_time, n, t);
}

///////////////////////////////////////////////////////
//----------------------- FCFS -------------------------
void findWaitingTimeFCFS(int processes[], int n, int burst_time[], int wt[], int &t)
{
	// el tiempo de espera del primer procesos es 0
	wt[0] = 0;
	// el tiempo de espera es tiempo en el que termina el proceso anterior
	for (int i = 1; i < n; i++)
		wt[i] = burst_time[i - 1] + wt[i - 1];
	t = wt[n-1] + burst_time[n- 1];
}

void FCFS(int processes[], int n, int burst_time[])
{
	int wt[160], tat[160], total_wt = 0, total_tat = 0, t = 0;

	// Calculamos el waiting time y el turnaround time de cada proceso
	findWaitingTimeFCFS(processes, n, burst_time, wt, t);
	findTurnAroundTime(processes, n, burst_time, wt, tat);

	cout << "\n\n\n_____________________________ FCSF _____________________________\n";

	printTable(wt, tat, burst_time, n, t);
}

// interfaz para modificar los datos de los procesos
void processModification(int processes[], int n, int burst_time[], int quantum)
{
	int id;
	cout << "\n\nEl burst time ha sido calculado aleatoriamente\n";
	char c;
	do
	{
		do
		{
			cout << "Desea cambiar el burst time de algun proceso? (s/n)\n";
			cin >> c;
			if (c == 's' || c == 'S')
			{
				cout << "Ingrese el ID del proceso a modificar:\n";
				do
				{
					cin >> id;
					if (id > n)
						cout << "ID no valido. Ingreselo nuevamente:\n";
				} while (id > n);
				cout << "Process " << id << " burst time: " << burst_time[id - 1] << "\n";
				cout << "Ingrese el nuevo burst time:\n";
				cin >> burst_time[id - 1];
			}
			else if (c != 'n' && c != 'N')
				cout << "Ingrese 's' o 'n'\n";
		} while (c != 'n' && c != 'N');

		// modificacion del time quantum
		cout << "\nDefault time quantum: " << quantum << endl;
		do
		{
			cout << "Desea cambiar el time quantum? (s/n)\n";
			cin >> c;
			if (c == 's' || c == 'S')
			{
				cout << "Ingrese el nuevo time quantum:\n";
				cin >> quantum;
			}
			else if (c != 'n' && c != 'N')
				cout << "Ingrese 's' o 'n'\n";
		} while (c != 'n' && c != 'N');
		// Burst time of all processes
		// int burst_time[] = {10, 5, 8, 90, 6, 2, 3};

		RR(processes, n, burst_time, quantum);
		FCFS(processes, n, burst_time);
		// cout<<"\nDifference of average waiting time: "<<awtFCFS-awtRR<<endl;
		// cout<<"Difference of average turn around time: "<<atatFCFS-atatRR<<endl<<endl;

		cout << "\nDesea seguir modificando el programa? (s/n)\n";
		cin >> c;

	} while (c != 'n' && c != 'N');
}

int main()
{
	srand(time(NULL));
	int n, processes[160], burst_time[160];

	cout << "Ingrese el numero de procesos (maximo 160):\n";
	cin >> n;
	for (int i = 0; i < n; i++)
	{
		// process id's
		processes[i] = i + 1;
		// burst time aleatorio den el rango 1-10
		burst_time[i] = (rand() % 10) + 1;
	}
	//se supone que todos los procesos tienen el mismo arrival time = 0
	int quantum = 5;
	RR(processes, n, burst_time, quantum);
	FCFS(processes, n, burst_time);

	// menu para modificar los datos de los procesos
	processModification(processes, n, burst_time, quantum);


	return 0;
}

