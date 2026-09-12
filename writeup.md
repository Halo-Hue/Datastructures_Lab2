The ownership rule for this list states that when a token is pushed into a list, the list that it is pushed into takes ownership
of it. Whoever owns the token is responsible for freeing its memory. In my implementation I transfer ownership of the token
to the list node after a successful push. When the token is no longer needed I free the token's lexeme before freeing the node,
otherwise if i were to free the node first, i would no longer be able to free the lexeme. For example, if I create a token 
and pass it to list_push_front, if the new node is allocated successfully the token is stored in that node and ownership is 
transferred to the list which is now responsible for freeing it when its no longer needed. If list_push_front failed to
allocate a new node, ownership would not be passed onto the list because the token isnt stored in a node, if this were to happen
it would call token_free in order to free the token before returning, this would prevent a memory leak. If the token was not freed
at this step its memory would be lost because there would be no node that owns it. 
