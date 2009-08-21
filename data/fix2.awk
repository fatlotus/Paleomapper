{if (NR==1) { lat=$1; lon=$2; pen=$3; print $0}
 else { if (lat!=$1 || lon!=$2) 
		print $0;
	lat=$1; lon=$2; 
      }
}
