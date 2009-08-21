{if (NR==1) { lat=$1; lon=$2; pen=$3}
 else { if (lat==$1 && lon==$2) 
		print NR-1,lat,lon,pen,"\n"NR,$0;
	lat=$1; lon=$2; 
      }
}
