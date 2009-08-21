	Character*80 name
	CHARACTER*3 REFNUM
	CHARACTER*2 TYPE
	character*1 tab
	tab=char(9)
	write(6,*) 'what existing file do you want to open?'
	read(5,1) name
	open (unit=2,name=name)
	write(6,*) 'what new file do you want to create?'
	read(5,1) name
	open (unit=3,name=name)
1	format(a80)
	write(6,*) 'what is the starting points lat and long'
	read(5,*) alat,along
	write(6,*) alat,tab,along
	xa=cosd(alat)*cosd(along)
	ya=cosd(alat)*sind(along)
	za=sind(alat)
2	read(2,1004,end=10)REFNUM,NEWSEQ,ID,INC,
     *T1,T2,TYPE,LTYP,ICOL
1004       FORMAT(1X,1A3,1I4,1X,1I3,1X,1I4,2(1x,f6.2),1X,1A2,1X,1i5,
     *1X,1I1)
3	read(2,*)blat,blong
	if(blat.eq.99.00) go to 2
	xb=cosd(blat)*cosd(blong)
	yb=cosd(blat)*sind(blong)
	zb=sind(blat)
	delta=acosd((xa*xb)+(ya*yb)+(za*zb))
	dist=delta/360.*2.0*3.1415927*6370.9
	write(3,*)T2,tab,dist,tab,LTYP
	go to 3
10	continue
	close unit=2
	close unit=3
	stop
	end
