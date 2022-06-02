Operating Systems Ex6

Ilan Sirisky
Eldad Tsemach

How to run:
'make all' in the terminal

1-3: run './main1' to run the server with the pipeline
     run './client localhost' for each client to insert 5 string to the main queue

4: run './guard'

5: run './singleton'

6: run './pollserver'
   run './reactorclient localhost' for each client 

Tests: 
1-3: run the commends above, from each client you will need to send 5 strings
    after a few seconds the client will recieve his string after the changes from the pipeline.

4: run the commend above, insert 2 numbers, you will see when the guard lock and after 2 seconds
   of sleep it will print the first number you inserted, only after the first number was printed
   the guard will unlock and try to print the second number.

5: run the commend above, you will see that both of the instances are containing the same information about the file

6: run the commends above, need at least 2 clients so they can communicate between themselves
   and not with the server.
   