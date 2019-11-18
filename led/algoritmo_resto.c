int resto,m_1,m_pc,m_sc,m_tc;
m=1250; //valore di prova
m_1=12412; //è 1241.2, ricordarsi di dividere sempre per 10
m_pc=int(m*10/m_1); //m prima cifra
resto=(m*10)%m_1;//resto
m_sc=int(resto*10/m_1);//m seconda cifra
resto=(resto*10)%m_1;//resto
m_tc=int(resto*10/m_1);//m terza cifra
