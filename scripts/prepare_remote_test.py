import os
import sys
import uuid
import paramiko
import digitalocean
from time import sleep
from datetime import datetime
from subprocess import check_call


DROPLET_NAME = 'test-{uuid}'.format(uuid=str(uuid.uuid4()))
SSH_KEY_NAME = 'key-{name}'.format(name=DROPLET_NAME)
DROPLET_STATUS_COMPLETED = 'completed'
BINTRAY_USERNAME = os.getenv('BINTRAY_USERNAME')


class DropletSetup(object):
    def __init__(self, token, size, region):
        super(DropletSetup, self).__init__()
        self.token = token
        self.size = size
        self.region = region
        self.ssh_client = paramiko.SSHClient()
        self.ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.ssh_key = None
        self.digital_ocean_ssh_key = None

        self._generate_ssh_key()
        self.droplet = digitalocean.Droplet(
            token=self.token,
            name=DROPLET_NAME,
            region=self.region,
            image='ubuntu-16-04-x64',
            size_slug=self.size,
            ssh_keys=[self.digital_ocean_ssh_key.id]
        )

    @staticmethod
    def _print_info(message):
        print('[{timestamp}]\t{message}'.format(timestamp=datetime.now().isoformat(), message=message))

    def _execute_command(self, command):
        self._print_info('executing {command}'.format(command=command))
        _, _, std_err = self.ssh_client.exec_command(command)

        #for line in std_out.readlines():
        #    print(line.replace('\n', ''))

        for line in std_err.readlines():
            print(line.replace('\n', ''))
            raise Exception('Script execution failed')

    def _generate_ssh_key(self):
        self._print_info('generating ssh key')
        self.ssh_key = paramiko.rsakey.RSAKey.generate(2048, str(uuid.uuid4()))

        self._print_info('create ssh key on DigitalOcean')
        self.digital_ocean_ssh_key = digitalocean.SSHKey(
            token=self.token,
            name=SSH_KEY_NAME,
            public_key='ssh-rsa {key}'.format(key=str(self.ssh_key.get_base64()))
        )

        self.digital_ocean_ssh_key.create()

    def create_droplet(self):
        self._print_info('creating droplet')
        self.droplet.create()

        self._print_info('waiting for droplet to be ready')
        self._wait_for_droplet()

    def _wait_for_droplet(self):
        actions = self.droplet.get_actions()
        for action in actions:
            if action.status == DROPLET_STATUS_COMPLETED:
                self.droplet.load()
                return

        self._wait_for_droplet()

    def connect(self):
        self._print_info('connecting to droplet')
        try:
            self.ssh_client.connect(
                hostname=self.droplet.ip_address,
                username='root',
                allow_agent=True,
                pkey=self.ssh_key
            )
        except Exception as exc:
            self._print_info(str(exc))
            self._print_info('reconnecting')
            sleep(3)
            self.connect()

    def install_rebirthdb(self):
        self._print_info('getting rebirthdb')
        self._execute_command('source /etc/lsb-release && echo "deb https://dl.bintray.com/{username}/apt $DISTRIB_CODENAME main" | tee /etc/apt/sources.list.d/rebirthdb.list'.format(username=BINTRAY_USERNAME))
        self._execute_command('wget -qO- https://dl.bintray.com/{username}/keys/pubkey.gpg | apt-key add -'.format(username=BINTRAY_USERNAME))

        self._print_info('installing rebirthdb')
        self._execute_command('apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y rebirthdb')
        self._execute_command('echo "bind=all" > /etc/rebirthdb/instances.d/default.conf')

    def start_rebirthdb(self):
        self._print_info('restarting rebirthdb')
        self._execute_command('/etc/init.d/rebirthdb restart')

    def run_script(self, script, script_arguments):
        self._print_info('executing script')
        os.environ["REBIRTHDB_HOST"] = self.droplet.ip_address
        check_call([script, ' '.join(script_arguments)])

    def cleanup(self):
        self._print_info('destroying droplet')
        self.droplet.destroy()

        self._print_info('removing ssh key')
        self.digital_ocean_ssh_key.destroy()


def main():
    script = sys.argv[1]
    script_arguments = sys.argv[2:]

    setup = DropletSetup(
        token=os.getenv('DO_TOKEN'),
        size=os.getenv('DO_SIZE', '512MB'),
        region=os.getenv('DO_REGION', 'sfo2')
    )

    setup.create_droplet()

    try:
        setup.connect()
        setup.install_rebirthdb()
        setup.start_rebirthdb()
        setup.run_script(script, script_arguments)
    except Exception as exc:
        print(str(exc))
    finally:
        setup.cleanup()


if __name__ == '__main__':
    main()
