use Pokemon;
select Trainer.name from CatchedPokemon,Trainer where CatchedPokemon.owner_id = Trainer.id group by owner_id Having count(*)>=3 order by count(owner_id) desc;
select name from Pokemon where type in ( select * from ( select type from Pokemon group by type order by count(*) desc,type limit 2)as t) order by name asc;
select name from Pokemon where name like '_o%' order by name asc;
select nickname from CatchedPokemon where level>=50 order by nickname asc;
select name from Pokemon where char_length(name) = 6 order by name asc;
select name from Trainer where hometown = 'Blue City' order by name asc;
select distinct hometown from Trainer order by hometown asc;
select avg(level) from CatchedPokemon join Trainer on Trainer.id=owner_id where Trainer.name = 'Red';
select nickname from CatchedPokemon where nickname not like 'T%' order by nickname asc;
select nickname from CatchedPokemon where level>=50 and owner_id>=6 order by nickname;
select id,name from Pokemon order by id asc;
select nickname from CatchedPokemon where level<=50 order by level asc;
select P.name, P.id from Pokemon P,CatchedPokemon C, Trainer T  where C.pid=P.id and C.owner_id=T.id and T.hometown = 'Sangnok City' order by P.id asc;
select C.nickname from Gym G,CatchedPokemon C,Pokemon P where G.leader_id=C.owner_id and  C.pid=P.id and P.type = 'Water' order by C.nickname asc;
select count(C.pid) as num from CatchedPokemon C, Evolution E where C.pid = E.before_id;
select count(*) from Pokemon where type = 'Water' or type = 'Electric' or type = 'Psychic';
select count(distinct C.pid) from Trainer T, CatchedPokemon C where T.id=C.owner_id and hometown = 'Sangnok City';
select max(level) from Trainer T, CatchedPokemon C where T.id=C.owner_id and hometown = 'Sangnok City';
select count(distinct P.type) from Gym G, CatchedPokemon C, Pokemon P where G.leader_id = C.owner_id and C.pid=P.id and G.city='Sangnok City';
select T.name,count(C.pid) from Trainer T, CatchedPokemon C where T.id=C.owner_id and hometown = 'Sangnok City' group by T.name order by count(C.pid) ;
select name from Pokemon where name like 'A%' or name like 'E%' or name like 'I%' or name like 'O%' or name like 'U%';
select type, count(type) from Pokemon group by type order by count(type), type;
select distinct T.name from Trainer T, CatchedPokemon C where T.id=C.owner_id and C.level<=10 order by T.name;
select City.name, avg(C.level) from City,CatchedPokemon C,Trainer T where City.name=T.hometown and C.owner_id=T.id group by City.name order by avg(C.level);
select distinct P.name from CatchedPokemon C1 join Trainer T1 on C1.owner_id=T1.id and T1.hometown='Sangnok City' , CatchedPokemon C2 join Trainer T2 on C2.owner_id=T2.id and T2.hometown='Brown City', Pokemon P  where C1.pid=C2.pid and P.id=C1.pid order by P.name asc;
select P.name from CatchedPokemon C,Pokemon P  where C.pid=P.id and nickname like '% %' order by P.name desc;
select T.name,max(C.level) from Trainer T,CatchedPokemon C where T.id=C.owner_id group by T.name having count(C.pid)>=4 order by T.name asc;
select T.name,avg(C.level) from CatchedPokemon C join Trainer T on T.id=C.owner_id join Pokemon P on P.id=C.pid where P.type='Normal' or P.type='Electric' group by T.name order by avg(C.level) asc;
select P.name, T.name, description from Pokemon P join CatchedPokemon C on P.id=C.pid join Trainer T on C.owner_id=T.id join City on City.name=hometown where P.id=152 order by C.level desc;
select P1.id, P1.name, P2.name, P3.name from Pokemon P1, Pokemon P2, Pokemon P3, Evolution E1, Evolution E2 where E1.after_id=E2.before_id and P1.id=E1.before_id and P2.id = E1.after_id and P3.id= E2.after_id order by P1.id asc;
select name from Pokemon where char_length(id)=2 order by name asc;
select P.name from Pokemon P left join CatchedPokemon C on P.id=C.pid where C.owner_id is null order by P.name asc;
select sum(C.level) from CatchedPokemon C, Trainer T where C.owner_id=T.id and T.name='Matis';
select T.name from Trainer T, Gym G where T.id=G.leader_id order by T.name asc;
select type , concat(count(*)/(select count(*) from Pokemon)*100,'%') as 'Rating' from Pokemon group by type order by count(*) desc limit 1;
select T.name from CatchedPokemon C,Trainer T where C.owner_id=T.id and C.nickname like 'A%' order by T.name asc;
select T.name,sum(C.level) from CatchedPokemon C, Trainer T where C.owner_id=T.id group by T.name order by sum(C.level) desc limit 1;
select P.name from Pokemon P, Evolution E where P.id=E.after_id and E.after_id not in (select x.after_id from Evolution x, Evolution y where x.before_id=y.after_id) order by P.name asc;
select T.name from Trainer T, CatchedPokemon C where T.id=C.owner_id group by T.id having count(C.pid)>count(distinct C.pid) order by T.name asc;
select T.hometown,C.nickname from Trainer T, CatchedPokemon C, (select hometown,max(level) as max from Trainer T1, CatchedPokemon C1 where T1.id=C1.owner_id group by hometown) as t where T.id=C.owner_id and T.hometown = t.hometown and C.level=t.max order by T.hometown asc;
