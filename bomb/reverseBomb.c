// pseudo code
phase_5(char *user_str)
{
 char offset[7];
 long i = 0;
 len = len(user_str);	// the len() is python's function, not c
 char string[7];
 char str_hidden[100] = "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?";
 if (len == 6)
 {
	 while(i != 6)
	 {
		 *(offset + i) = *(user_str + i) & 0xf;
		 *(string + i) = *(str_hidden + *(offset + i));
		 i++;
	 }
	 offset[6] = '\0';
 }
 char target[7]="flyers";	// addr of target is only provided by bomb.s
 if (! string_not_equal(string, target))
	 Done;

