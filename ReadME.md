Abordarea generala:
Burnichi Alexandra, 324CC



	In tema incarcata, am incercat sa realizez un cod cat mai inteligibil, succint si eficient pentru a implementa un loader de fisiere executabile in format ELF pentru Linux. 
	Pentru a reusi rezolvarea temei, am utilizat structuri de date prezente in cerinta temei, paginile de manual ale functiilor si informatiile din cursuri si laboratoare.
	Am cautat in fiecare pagina a fiecarui segment al fisierului adresa Segmentation fault-ului si unde eroarea era cauzata de o pagina nemapata, am mapat pagina.
	Daca eroarea nu este de mapare si adresa de Segmentation fault nu s a gasit in cadrul niciunui segment, am folosit handler-ul vechi.
