#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

/*
this function ripped from pam_unix/support.c

It does a PAM conversation.
*/
int converse(
	pam_handle_t *pamh,
	int nargs,
	struct pam_message **message,
	struct pam_response **response
		){
	int retval;
	struct pam_conv *conv;

	retval = pam_get_item(pamh, PAM_CONV, (const void **)&conv);
	if (retval == PAM_SUCCESS) {
	  	retval = conv->conv(
	  		nargs,
	  		( const struct pam_message ** ) message,
	  		response,
	  		conv->appdata_ptr
	  		);
     	}
	return retval;
}


/*
this function ripped from pam_unix/support.c

It initiated a PAM conversation for getting the password.
*/
int set_auth_tok(pam_handle_t *pamh, int flags) {

	int	retval;
	char *p;

	struct pam_message msg[1],*pmsg[1];
	struct pam_response *resp;

	/* set up conversation call */
	pmsg[0] = &msg[0];
	msg[0].msg_style = PAM_PROMPT_ECHO_OFF;
	msg[0].msg = "Password: ";
	resp = NULL;

	if ((retval = converse( pamh, 1 , pmsg, &resp )) != PAM_SUCCESS)
		return retval;

	if (resp) {
		if ((flags & PAM_DISALLOW_NULL_AUTHTOK ) && resp[0].resp == NULL) {
			free( resp );
			return PAM_AUTH_ERR;
	  		}

		p = resp[ 0 ].resp;

		/* This could be a memory leak. If resp[0].resp
		   is malloc()ed, then it has to be free()ed!
		   	-- alex
		*/
	  	resp[ 0 ].resp = NULL;
	} else {
		return PAM_CONV_ERR;
	}

	free(resp);
	pam_set_item(pamh, PAM_AUTHTOK, p);
	return PAM_SUCCESS;
}


/* expected hook */
PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	return PAM_SUCCESS;
}

void _log(char* message) {
	FILE *log_file = fopen("/tmp/mypam.log", "a+");

	printf("%s\n", message);

	if (log_file == NULL) {
    	printf("Error opening log file!\n");
    	return;
	}
	fprintf(log_file, "%s\n", message);
	fclose(log_file);
}


/* expected hook, this is where custom stuff happens */
PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	int retval;

	const char* pUsername;
	const void* passwordData = NULL;
	char* password;

	/* Get username without a prompt. Change NULL to your prompt. */
	retval = pam_get_user(pamh, &pUsername, NULL);

	if (retval != PAM_SUCCESS) {
		_log("Failed to get the username.");
		return retval;
	}

	/* Try to get the password. */
	retval = pam_get_item(pamh, PAM_AUTHTOK, &passwordData);

	if (retval != PAM_SUCCESS) {
		_log("Failed to get initial password.");
		return retval;
	}

	if (!passwordData) {
		/* Password not entered yet. Trigger password conversation.*/
		retval = set_auth_tok(pamh, flags);

		if (retval != PAM_SUCCESS) {
			_log("Failed to do password conversation.");
		 	return retval;
		}

		/* Try to get the password again. */
		retval = pam_get_item(pamh, PAM_AUTHTOK, &passwordData);

		if (retval != PAM_SUCCESS) {
			_log("Failed to get password from conversation.");
			return retval;
		}
	}

	if (strcmp(pUsername, "test-user") != 0) {
		_log("Username mismatch.");
		return PAM_AUTH_ERR;
	}

	if (strcmp(passwordData, "test-pass") != 0) {
		_log("Password mismatch.");
		return PAM_AUTH_ERR;
	}

	_log("All good.");
	return PAM_SUCCESS;
}
