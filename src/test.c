#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>

const struct pam_conv conv = {
	misc_conv,
	NULL
};

int main(int argc, char *argv[]) {
	pam_handle_t* pamh = NULL;
	int retval;
	const char* user = "nobody";
	const char* service = "login";

	if(argc != 3) {
		printf("Usage: test service username\n");
		exit(1);
	}

	service = argv[1];
	user = argv[2];

	retval = pam_start(service, user, &conv, &pamh);

	// Are the credentials correct?
	if (retval == PAM_SUCCESS) {
		retval = pam_authenticate(pamh, 0);
	}

	// Did everything work?
	if (retval == PAM_SUCCESS) {
		printf("Authenticated\n");
	} else {
		printf("Not Authenticated\n");
	}

	// Can the account be used at this time?
	retval = pam_acct_mgmt(pamh, 0);
	if (retval == PAM_SUCCESS) {
		printf("Account is valid.\n");
	} else {
		printf("Account is denied.\n");
	}


	// close PAM (end session)
	if (pam_end(pamh, retval) != PAM_SUCCESS) {
		pamh = NULL;
		printf("check_user: failed to release authenticator\n");
		exit(1);
	}

	return retval == PAM_SUCCESS ? 0 : 1;
}
