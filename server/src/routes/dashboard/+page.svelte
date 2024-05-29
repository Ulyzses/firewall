<script lang="ts">
  interface Device {
    id: number;
    name: string;
    mac: string;
    status: boolean;
    smoke: boolean;
  }

  export let data;

  $: ({ user, supabase, devices } = data);

  let addDeviceForm: HTMLFormElement;

  async function addDevice() {
    const formData = new FormData(addDeviceForm);
    const macAddress = formData.get('macAddress') as string;

    const { data, error } = await supabase
      .from('devices')
      .insert({
        user: user?.id,
        mac: macAddress
      })
      .select();

    if (error) {
      console.error(error);
    } else {
      console.log(data);
    }
  }

  async function toggleDevice(device: Device) {
    const { data, error } = await supabase
      .from('actions')
      .insert({
        mac: device.mac,
        action: device.status ? 'OFF' : 'ON'
      })
      .select();
    
    if (error) {
      console.error(error);
    } else {
      console.log(data);
    }

    device.status = !device.status;
    devices = devices;
  }
</script>

<form bind:this={addDeviceForm} on:submit|preventDefault={addDevice}>
  <label for="macAddress">
    <input name="macAddress" type="text" />
  </label>
  <button>Add Device</button>
</form>

{#each devices as device (device.id)}
  <div>
    <span>{device.id}</span>
    <span>{device.name}</span>
    <span>{device.mac}</span>
    <span>{device.status}</span>
    {#if device.smoke}
      <span>Smoke Detected</span>
    {/if}
    <button on:click={() => toggleDevice(device)}>{device.status ? 'Turn Off' : 'Turn On'}</button>
  </div>
{/each}

<!-- top bar -->
<!-- FireWall name -->
<!-- user icon -->
<!-- logout -->
<!-- devices -->
<!-- title -->
<!-- add device -->
<!-- device list -->
<!-- smoke detected -->
<!-- plug icon -->
<!-- name, number -->
<!-- status -->
