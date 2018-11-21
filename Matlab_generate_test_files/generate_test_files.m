output_file=fopen('c:\temp\pipe4.txt','w');

for i=1:5000
    if ((i>1000)&(i<2000))
        array(i)=(i-1000)*0;
    else
        array(i)=0;
    end
end

for i=1:5000
    fprintf(output_file,'%i\n',array(i));
end

fclose(output_file);

