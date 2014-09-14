package startat.de.thegame.util.ecs;

public abstract class Component {

	protected boolean active = false;
	
	protected Entity entity = null;
	
	protected String type;
	
	
	public Component(String type){
		this.type = type;
	}
	
	public String getType() {
		return type;
	}
	
	public void setEntity(Entity entity) {
		this.entity = entity;
	}
	
	public Entity getEntity() {
		return entity;
	}
	
	public abstract void init();
	public abstract void kill();


}
