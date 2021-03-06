#include "functions.hpp"


vector<Jeek*> User::get_jeeks() 
{
	return jeeks ;
}

string User::get_display_name () 
{ 
	return display_name ;
}

string User::get_password () 
{
	return password ;
}

string User::get_username () 
{ 
	return username ;
}

vector<Reply*> User::get_replies() 
{ 
	return replies; 
}

void User::add_reply (Network & network) 
{
	string comment_id , content ;
	cin >> comment_id >> content ;
	Comment* the_comment = find_comment ( comment_id , network );
	Reply * the_reply = find_reply ( comment_id , network ) ;
	if ( the_reply == NULL && the_comment == NULL ) {
		cout << "comment/reply not found" << endl;
		return ;
	}
	Reply* new_reply = new Reply ;
	new_reply -> set_features( this , content );
	replies.push_back(new_reply);
	network.add_reply(new_reply);
	if ( the_reply == NULL ) {
		the_comment -> add_reply (new_reply);
		the_comment -> get_author() -> add_notifications ( this -> get_username() + " replied " + the_comment -> get_id() );
	}
	else the_reply -> add_reply (new_reply);
	cout << "reply succeesully posted" << endl;
}

void User::add_comment (Network & network) 
{
	string jeek_id , content;
	cin >> jeek_id >> content ;
	Jeek * jeek = find_jeek ( jeek_id , network );
	if ( jeek == NULL ) {
		cout << "the jeek you wanted to comment on doesn't exist" << endl;
		return ;
	}
	Comment * new_comment = new Comment;
	new_comment -> set_features ( content , username + to_string ( comments.size() ) , this );
	network.add_comment (new_comment);
	comments.push_back (new_comment);
	jeek -> add_comment (new_comment);
	jeek -> get_author() -> add_notifications ( this -> get_username() + " commented on " + jeek -> get_id() );
	cout << "comment successfully posted" << endl;
}

void User::add_jeek( vector<User*> &users , Network & network ) 
{
	string command = "" , content = "" ;
	Jeek *new_jeek = new Jeek ;
	new_jeek -> set_features ( "" , username + to_string ( jeeks.size() ) , this ) ;
	while ( command != "publish" && command != "abort" ) {
		cin >> command;
		if ( command == "text") {
			getline ( cin , content ) ;
			if ( content.size() == 0 )  continue ;
			content.erase(content.begin());
			new_jeek->set_text ( content );
		}
		else if ( command == "tag" ) {
			getline ( cin , content );
			if ( content.size() == 0 ) continue ; 
			content.erase(content.begin());
			new_jeek->add_a_hashtag (content , network );
		}
		else if ( command == "mention" ) {
			getline ( cin , content );
			if ( content.size() == 0 ) continue ;
			content.erase(content.begin());
			new_jeek->mention(content, users , this );
		}
	}
	if ( command == "abort" || new_jeek -> get_text() == "" ) {
		cout << "adding jeek failed due to not having any text or entering abort command by user " << endl;
		return ;
	} 
	jeeks.push_back(new_jeek);
	network.add_jeeks (new_jeek);
	add_notifications_for_followers_after_jeek();
	cout << "jeek successfully published" << endl;
}

void User::set_session_Id (string Id)
{
	session_Id = Id ;
}

void User::set_features ( string userName , string displayName , string passWord ) 
{
	username = userName ;
	display_name = displayName ;
	password = passWord ;
	session_Id = "";
}

void User::add_rejeeks ( Jeek* new_jeek ) 
{
	jeeks.push_back(new_jeek);
}

void User::show_jeeks () 
{
	for ( int i = 0 ; i < jeeks.size() ; i++ )
		jeeks[i]->show_after_search();
}

bool User::following_does_not_exist ( User* user_to_be_checked ) 
{
	for ( int i = 0 ; i < followings.size() ; i++ ) 
		if ( user_to_be_checked -> get_username() == followings[i] -> get_username() ) return false;
	return true;
}

void User::follow ( User* user_to_get_followed ) 
{
	followings.push_back(user_to_get_followed);
}

int User::find_following_index ( User* user_to_be_found ) 
{
	for ( int i = 0 ; i < followings.size()  ; i++ )
		if ( user_to_be_found -> get_username() == followings[i] -> get_username() ) return i ;
	return -1 ;
}

void User::unfollow ( User* user_to_get_unfollowed ) 
{
	followings.erase( followings.begin() + find_following_index ( user_to_get_unfollowed ) );
}

bool User::like_does_not_exist ( Jeek* jeek_to_get_checked ) {
	for ( int i = 0 ; i < likes.size() ; i++ ) 
		if ( likes[i] -> get_id() == jeek_to_get_checked -> get_id() ) return false;
	return true;
}

void User::like( Jeek* jeek_to_get_liked ) 
{
	jeek_to_get_liked -> get_liked ();
	likes.push_back(jeek_to_get_liked);
}

int User::find_likes_index ( Jeek* jeek_to_get_found ) 
{
	for ( int i = 0 ; i < likes.size() ; i++ ) 
		if ( likes[i] -> get_id() == jeek_to_get_found -> get_id () ) return i;
	return -1 ;
}

void User::dislike ( Jeek* jeek_to_get_disliked ) 
{
	jeek_to_get_disliked -> get_disliked();
	likes.erase ( likes.begin() + find_likes_index (jeek_to_get_disliked) );
}

void User::add_notifications ( string notification ) 
{
	notifications.push_back(notification);
}

vector<User*> User::get_followings () 
{ 
	return followings ; 
}

void User::add_follower ( User* follower ) 
{
	followers.push_back(follower);
}

int User::find_follower_index ( User* former_follower ) 
{
	for ( int i = 0 ; i < followers.size() ; i++ ) 
		if ( followers[i] -> get_username() == former_follower -> get_username() ) return i;
	return -1;
}

void User::delete_follower ( User* former_follower )
{
	followers.erase ( followers.begin() + find_follower_index (former_follower) ) ;
}

void User::add_notifications_for_followers_after_jeek () 
{
	for ( int i = 0 ; i < followers.size() ; i++ ) 
		followers[i] -> add_notifications ( this -> get_username() + " jeeked " + ( jeeks[jeeks.size() - 1 ] -> get_id() ) ) ;
}

void User::show_notifications () 
{
	for ( int i = 0 ; i < notifications.size() ; i++ )
		cout << notifications[i] << endl ;
	notifications.erase( notifications.begin() , notifications.end() );
}

string User::get_session_id () 
{
	return session_Id ;
}